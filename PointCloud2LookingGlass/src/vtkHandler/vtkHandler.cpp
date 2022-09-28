#include "vtkHandler.h"

enum saveJSONMode { config, profile };

double cameraClippingRangeNear = 0.0, cameraClippingRangeFar = 0.0;
double cameraViewAngle = 0.0, cameraWindowCenter[2] = { 0.0, 0.0 }, cameraFocalDistance = 0.0;
int actualThreads = thread::hardware_concurrency() * 2 / 3;
bool newProfileCreated = false, newConfigCreated = false, defaultValueUpdate = false;
string finalReaderPath;
string finalConfigFileName;
string finalProfileName;
string configFileName;
vector<string> videoFileList;

Util::pathResult mediaPath;
Util::pathResult programPath;

jsonHandler::profileCamera jsonProfileData;
jsonHandler::mediaConfig jsonConfigData;

void singlePLYPlot();
void showVideo(int);
void plyMesh(const char*, const char*);
void generalPipeline();
void lookingGlassPipeline();
bool isNewProfileCreated(const string);
void saveCameraSettingsToJSON(string, saveJSONMode);
void loadCameraSettingsFromJSON();
void initializeConfig();
void configHandler();
void multithreadRequest();
void interactiveStyle();

namespace something {
    // Define interaction style
    class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera {
    public:
        static KeyPressInteractorStyle* New();
        vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);
        vtkRenderer* renderer;
        vtkRenderWindow* renderWindow;
        enum cameraClippingPlaneSelection { none, near, far, focalpoint };
        cameraClippingPlaneSelection camClipSel = none;
        //jsonHandler::profileCamera* jsonProfileData;
        virtual void OnKeyPress() override;
    };
    vtkStandardNewMacro(KeyPressInteractorStyle);

    class videoFrames : public vtkAnimationCue {
    public:
        vtkTypeMacro(videoFrames, vtkAnimationCue);
        static videoFrames* New();
        vtkSmartPointer<vtkPLYReader> reader;
        vtkSmartPointer<vtkRenderWindow> renderWindow;
        string filePath;
    private:
        virtual void TickInternal(double, double, double);
    };
    vtkStandardNewMacro(videoFrames);

    class vtkTimerCallback2 : public vtkCommand
    {
    public:
        vtkTimerCallback2() = default;
        ~vtkTimerCallback2() = default;

        int timerId = 0;
        static vtkTimerCallback2* New()
        {
            vtkTimerCallback2* cb = new vtkTimerCallback2;
            cb->TimerCount = 0;
            cb->allActors = cb->renderer->GetActors();
            cb->nextActor = nullptr;
            cb->previousActor = nullptr;
            cb->allActors->InitTraversal(cb->actorIndex);
            return cb;
        }
        virtual void Execute(vtkObject* caller, unsigned long eventId,
            void* vtkNotUsed(callData));
    private:
        int TimerCount = 0;
        vtkActorCollection* allActors;
        vtkCollectionSimpleIterator actorIndex;
        vtkActor* nextActor;
        vtkActor* previousActor;

    public:
        vtkPLYReader* reader;
        vtkRenderer* renderer;
        vtkRenderWindow* renderWindow;
        
    };
    //vtkStandardNewMacro(vtkTimerCallback2);
}

vtkNew<vtkPLYReader> reader;
vtkNew<vtkPolyDataMapper> mapper;
vtkNew<vtkActor> actor;
vtkNew<vtkRenderer> renderer;
vtkNew<vtkRenderWindow> renderWindow;
vtkNew<vtkRenderWindowInteractor> iren;
vtkNew<something::KeyPressInteractorStyle> style;
vtkNew<vtkActorCollection> allActors;

time_t result = time(nullptr);

void interactorUsage() {
    printf("----------Interactor Usage----------\n");
    printf("Use arrow keys to rotate: Up & Down (Vertically) and Left & Right (Horizontally)\n");
    printf("Use WASD to move: W & S (Vertically) and A & D (Horizontally)\n");
    printf("i - Zoom [I]n\n");
    printf("o - Zoom [O]ut\n");
    printf("c - Switch between [C]lipping planes and focal plane\n");
    printf("l - Move selected plane closer\n");
    printf("p - Move selected plane further\n");
    printf("f - Overwrite default configeration values in camera pro[F]ile\n");
    printf("r - [R]eset prospective\n");
    printf("q - [Q]uit\n");
    printf("------------------------------------\n");
}

void vtkHandler::vtkHandler(char* argv[]) {
    programPath = Util::pathParser(argv[0], ".exe");        // Just need the directory, for mac or linux, isFile should be false
    if (strcmp(argv[1], "-v") == 0)     mediaPath = Util::pathParser(argv[2], ".ply");
    else                                mediaPath = Util::pathParser(argv[1], ".ply");

    // To prevent user input processed file
    int tempLen = mediaPath.fileName.length();
    const char* tempChar = &mediaPath.fileName.c_str()[tempLen - 6];
    if (mediaPath.isFile)    if (strcmp(tempChar, "meshed") == 0)     mediaPath.fileName.erase(tempLen - 6);

    generalPipeline();
    lookingGlassPipeline();
    interactiveStyle();
    configHandler();
    
    reader->SetFileName(finalReaderPath.c_str());
    reader->Update();
    loadCameraSettingsFromJSON();   // camera == camera values (for interactor) == struct

    interactorUsage();

    if (mediaPath.isFile)           singlePLYPlot();
    else {
        if (newConfigCreated) {
            singlePLYPlot();        // User changes zoom in/ clipping range on the first frame, then reload the video folder and play in normal mode
            saveCameraSettingsToJSON(finalConfigFileName, saveJSONMode::config);
        }
        loadCameraSettingsFromJSON();
        showVideo(jsonConfigData.FPS);
    }
    saveCameraSettingsToJSON(finalConfigFileName, saveJSONMode::config);
    if(newProfileCreated)   saveCameraSettingsToJSON(finalProfileName, profile);
}

void singlePLYPlot() {
    renderWindow->Render();
    iren->Start();
    renderWindow->Finalize();
}

void showVideo(int FPS) {
    renderer->RemoveActor(actor);
    
    int progressPreviousPercent = 0;
    int percent = 0, progress = 0;
    progressbar bar(100);
    bar.reset();
    bar.update();       // initial 0%
    for (int i = 0; i < videoFileList.size(); i++) {
        vtkNew<vtkPLYReader> reader;
        vtkNew<vtkPolyDataMapper> mapper;
        vtkNew<vtkActor> actor;
        //cout << chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() << ": Reading file " << i << endl;
        reader->SetFileName(videoFileList[i].c_str());
        reader->Update();
        mapper->SetInputConnection(reader->GetOutputPort());
        actor->SetMapper(mapper);
        actor->VisibilityOff();
        allActors->AddItem(actor);
        progress++;
        // progress bar
        percent = progress * 100 / videoFileList.size();
        if (percent > progressPreviousPercent) {
            bar.update();
            progressPreviousPercent = percent;  // update
        }
    }
    
    allActors->InitTraversal();
    for (int i = 0; i < allActors->GetNumberOfItems(); i++) {
        vtkActor* anActor = allActors->GetNextActor();
        //anActor->VisibilityOff();
        anActor->VisibilityOn();
        renderer->AddActor(anActor);
        renderWindow->Render();
        this_thread::sleep_for(chrono::milliseconds(1000/FPS));
        anActor->VisibilityOff();
    }
    
    /*
    renderWindow->Render();

    // Initialize must be called prior to creating timer events.
    iren->Initialize();

    // Sign up to receive TimerEvent
    vtkNew<something::vtkTimerCallback2> cb;
    cb->reader = reader;
    cb->renderer = renderer;
    cb->renderWindow = renderWindow;

    iren->AddObserver(vtkCommand::TimerEvent, cb);
    int timerId = iren->CreateRepeatingTimer(1000 / FPS);
    cb->timerId = timerId;

    // Start the interaction and timer
    iren->Start();
    */
    
    renderWindow->Finalize();
}

void showVideoBackUp(int FPS) {
    //if (newConfigCreated)    singlePLYPlot();       // To use interactor module to adjust camera settings and save
    vtkNew<vtkAnimationScene> scene;

    renderWindow->Render();

    scene->SetModeToSequence();
    scene->SetFrameRate(FPS);
    scene->SetStartTime(0);
    scene->SetEndTime(videoFileList[0].size() / FPS);

    for (int frameIndex = 0; frameIndex < videoFileList[0].size(); frameIndex++) {
        vtkNew<something::videoFrames> frame;
        frame->filePath = videoFileList[frameIndex];
        frame->reader = reader;
        frame->renderWindow = renderWindow;
        frame->SetTimeModeToNormalized();
        frame->SetStartTime((double)(frameIndex / videoFileList.size()));
        frame->SetEndTime((double)((frameIndex + 1) / videoFileList.size()));
        scene->AddCue(frame);
        //printf("Adding frame %d from %s\n", frameIndex+1, frame->filePath.c_str());
    }

    scene->Play();
    scene->Stop();
}

void something::KeyPressInteractorStyle::OnKeyPress() {
    // Get the keypress
    vtkRenderWindowInteractor* rwi = this->Interactor;
    std::string key = rwi->GetKeySym();

    // Handle an arrow key
    if (key == "Up") {
        renderer->GetActiveCamera()->Elevation(-5);
        renderer->GetActiveCamera()->OrthogonalizeViewUp();
    }
    if (key == "Down") {
        renderer->GetActiveCamera()->Elevation(5);
        renderer->GetActiveCamera()->OrthogonalizeViewUp();
    }
    if (key == "Left") {
        renderer->GetActiveCamera()->Azimuth(5);
    }
    if (key == "Right") {
        renderer->GetActiveCamera()->Azimuth(-5);
    }

    // Handle a "normal" key
    if (key == "i") {
        cameraViewAngle -= 2;
    }
    if (key == "o") {
        cameraViewAngle += 2;
    }
    if (key == "w") {
        cameraWindowCenter[1] -= 0.1;
    }
    if (key == "a") {
        cameraWindowCenter[0] += 0.1;
    }
    if (key == "s") {
        cameraWindowCenter[1] += 0.1;
    }
    if (key == "d") {
        cameraWindowCenter[0] -= 0.1;
    }
    if (key == "l") {
        if (camClipSel == none)     printf("No plane selected. \n");
        else if (camClipSel == near)    cameraClippingRangeNear -= 0.5;
        else if (camClipSel == far)     cameraClippingRangeFar -= 0.5;
        else if (camClipSel == focalpoint)  cameraFocalDistance -= 0.1;
    }
    if (key == "p") {
        if (camClipSel == none)     printf("No plane selected. \n");
        else if (camClipSel == near)    cameraClippingRangeNear += 0.5;
        else if (camClipSel == far)     cameraClippingRangeFar += 0.5; 
        else if (camClipSel == focalpoint)  cameraFocalDistance += 0.1;
    }
    if (key == "c") {
        switch (camClipSel) {
        case none:
            camClipSel = near;
            printf("Camera Clipping Plane Near selected.\n");
            break;
        case near:
            camClipSel = far;
            printf("Camera Clipping Plane Far selected.\n");
            break;
        case far:
            camClipSel = focalpoint;
            printf("Camera Focal Point Selected.\n");
            break;
        case focalpoint:
            camClipSel = none;
            printf("Camera Clipping Plane Unselected.\n");
            break;
        default:
            camClipSel = none;
            printf("Camera Clipping Plane Unselected.\n");
        }
    }
    if (key == "f") {
        cout <<  "Update to default values in camera profile? (y/n): " << endl;
        char choice = 'n';
        if (choice == 'y' || choice == 'Y') {
            defaultValueUpdate = true;
            renderer->GetActiveCamera()->GetPosition(jsonProfileData.defaultCameraPosition);
            renderer->GetActiveCamera()->GetViewUp(jsonProfileData.defaultViewUpVector);
            saveCameraSettingsToJSON(finalProfileName, profile);
        }
        // Switch off "overrideEnable" in config file
        jsonConfigData.overrideEnable = false;
    }

    if (key == "r") {  //TODO: Change 100, 001 to values before sample is loaded
        renderer->GetActiveCamera()->SetPosition(1, 0, 0);
        renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
        renderer->ResetCamera();
        renderer->GetActiveCamera()->GetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);
        cameraViewAngle = renderer->GetActiveCamera()->GetViewAngle();
        renderer->GetActiveCamera()->GetWindowCenter(cameraWindowCenter);
        cameraFocalDistance = renderer->GetActiveCamera()->GetDistance();
    }
    else {
        renderer->GetActiveCamera()->SetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);
        renderer->GetActiveCamera()->SetViewAngle(cameraViewAngle);
        renderer->GetActiveCamera()->SetWindowCenter(cameraWindowCenter[0], cameraWindowCenter[1]);
        renderer->GetActiveCamera()->SetDistance(cameraFocalDistance);
    }

    renderWindow->Render();

    // Forward events
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void something::videoFrames::TickInternal(double currenttime, double deltatime, double clocktime) {
    reader->SetFileName(this->filePath.c_str());
    reader->Update();
    renderWindow->Render();
}

// It may lose normal/vector information
void plyMesh(const char* srcFile, const char* destFile) {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(srcFile);
    reader->Update();
    vtkNew<vtkDelaunay2D> meshAlgo;
    meshAlgo->SetInputData(reader->GetOutput());
    meshAlgo->SetAlpha(0.075);
    meshAlgo->Update();
    vtkNew<vtkPLYWriter> writer;
    writer->SetFileName(destFile);
    if (Util::findInPLY(srcFile, Util::alpha))   writer->SetArrayName("RGBA");
    else writer->SetArrayName("RGB");
    writer->SetInputConnection(meshAlgo->GetOutputPort());
    writer->Write();
}

void generalPipeline() {
    mapper->SetInputConnection(reader->GetOutputPort());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);
    renderWindow->AddRenderer(renderer);
}

void interactiveStyle() {
    style->renderer = renderer;
    style->renderWindow = renderWindow;
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);
}

void lookingGlassPipeline() {
    // create the basic VTK render steps
    vtkNew<vtkRenderStepsPass> basicPasses;

    // create the looking glass pass
    vtkNew<vtkLookingGlassPass> lgpass;

    // initialize it
    lgpass->GetInterface()->Initialize();
    lgpass->SetDelegatePass(basicPasses);

    // setup the window based on information from looking glass
    int w, h;
    lgpass->GetInterface()->GetDisplaySize(w, h);
    renderWindow->SetSize(w, h);
    int x, y;
    lgpass->GetInterface()->GetDisplayPosition(x, y);
    renderWindow->SetPosition(x, y);
    renderWindow->BordersOff();

    // tell the renderer to use our render pass pipeline
    vtkOpenGLRenderer* glrenderer = vtkOpenGLRenderer::SafeDownCast(renderer);
    glrenderer->SetPass(lgpass);
}

bool isNewProfileCreated(const string programDir) {
    bool result = false;
    
    vector<string> profileList = Util::scanFileByType(programDir.c_str(), ".json");
    cout << "Please identify the source of this PLY model. Choose a number..." << endl;
    int index = 0;
    if (profileList.size() != 0) {
        for (index = 0; index < profileList.size(); index++) {
            Util::pathResult jsonFiles = Util::pathParser(profileList[index].c_str(), ".json");
            cout << index + 1 << ". " << &jsonFiles.fileName.c_str()[strlen("profile_")] << endl;        // i.e. Should show "kinect" from "profile_kinect.json"
        }
    }
    cout << index + 1 << ". [Create a new profile].. \nChoice (only number): ";
    int choice = 0;
    while (true) {
        cin >> choice;
        choice--;       // To be more humanized interaction, the user input should be 1 and above. 
        if (choice > profileList.size() || choice < 0)    cout << "Invalid parameter. " << endl;
        else break;
    }
    if (choice == profileList.size()) {     // meaning to create
        cout << "Enter Profile Name (i.e. Kinect): ";
        cin >> jsonConfigData.source;
        result = true;
    }
    else {
        Util::pathResult jsonFiles = Util::pathParser(profileList[choice].c_str(), ".json");
        jsonConfigData.source = &jsonFiles.fileName[strlen("profile_")];
        result = false;
    }
    finalProfileName = programDir + "profile_" + jsonConfigData.source + ".json";
    return result;
}

void initializeConfig() {
    newConfigCreated = true;
    newProfileCreated = isNewProfileCreated(programPath.directory);         // source value is also assigned
    string plyFile;
    if (mediaPath.isFile) {
        jsonConfigData.mediaType = "single";
        plyFile = mediaPath.directory + mediaPath.fileName + ".ply";
    }
    else {
        jsonConfigData.mediaType = "video";
        videoFileList = Util::scanFileByType(mediaPath.directory.c_str(), ".ply");
        plyFile = videoFileList[0];
        cout << "Video Directory detected. Please set Frame Per Second (i.e. 30): ";
        cin >> jsonConfigData.FPS;
    }
    finalReaderPath = plyFile;  // Will update if there is any face construction

    // For color
    if (!Util::findInPLY(plyFile.c_str(), Util::color)) {
        jsonConfigData.hasColor = false;
        cout << "Warning: no color information detected. " << endl;
    }
    else  jsonConfigData.hasColor = true;

    // For mesh
    finalConfigFileName = mediaPath.directory + mediaPath.fileName + ".json";       // If source not meshed, this will update
    reader->SetFileName(plyFile.c_str());
    reader->Update();
    string destFile;
    if (reader->GetOutput()->GetNumberOfPolys() == 0) {
        jsonConfigData.isSrcHasFaceEmbed = false;
        cout << "No face elements found. Constructing..." << endl;
        if (mediaPath.isFile) {
            destFile = mediaPath.directory + mediaPath.fileName + "meshed.ply";
            plyMesh(plyFile.c_str(), destFile.c_str());
            finalReaderPath = destFile;
            finalConfigFileName = mediaPath.directory + mediaPath.fileName + "meshed.json";     // Update
        }
        else {
            string destDir = mediaPath.directory;
#ifdef osWindows
            destDir += "meshed\\";
#elif osMacOS
            destDir += "meshed/";
#elif osLinux
            destDir += "meshed/";
#endif
            filesystem::create_directory(destDir);
            progressbar bar(100);
            int progressPreviousPercent = 0;
            bar.reset();
            bar.update();
            for (int i = 0; i < videoFileList.size(); i++) {
                Util::pathResult meshFileName = Util::pathParser(videoFileList[i].c_str(), ".ply");
                string destFile = destDir + meshFileName.fileName + "meshed.ply";
                plyMesh(videoFileList[i].c_str(), destFile.c_str());
                // progress bar
                int percent = i * 100 / videoFileList.size();
                if (percent > progressPreviousPercent) {
                    bar.update();
                    progressPreviousPercent = percent;  // update
                }
            }
            cout << endl;       // give a newline when bar.update reaches 100
            // rescan for meshed
            videoFileList = Util::scanFileByType(destDir.c_str(), ".ply");
            finalReaderPath = videoFileList[0];
            //finalConfigFileName = mediaPath.directory + "videoConfig.json";   // Since path and name are fixed, it is defined in vtkHandler
        }
    }
    else  jsonConfigData.isSrcHasFaceEmbed = true;

    jsonConfigData.overrideEnable = false;
    // jsonConfigData should now have: source, mediaType, hasColor, FPS, and isSrcHasFaceEmbed.
}

void loadCameraSettingsFromJSON() {
    if (newProfileCreated) {        // default -> camera 
        jsonProfileData.defaultCameraPosition[0] = 1.0;
        jsonProfileData.defaultCameraPosition[1] = 0.0;
        jsonProfileData.defaultCameraPosition[2] = 0.0;
        jsonProfileData.defaultViewUpVector[0] = 0.0;
        jsonProfileData.defaultViewUpVector[1] = 0.0;
        jsonProfileData.defaultViewUpVector[2] = 1.0;
        renderer->GetActiveCamera()->SetPosition(jsonProfileData.defaultCameraPosition);
        renderer->GetActiveCamera()->SetViewUp(jsonProfileData.defaultViewUpVector);
    }
    else {      // coresponding data should be loaded from json and exist in struct, struct -> camera
        if (jsonConfigData.overrideEnable) {
            renderer->GetActiveCamera()->SetPosition(jsonConfigData.overrideCameraPosition);
            renderer->GetActiveCamera()->SetViewUp(jsonConfigData.overrideViewUp);
        }
        else {
            string profilePath = programPath.directory + "profile_" + jsonConfigData.source + ".json";
            ifstream profileInput(profilePath);
            json j = json::parse(profileInput);
            jsonProfileData = j.get<jsonHandler::profileCamera>();
            renderer->GetActiveCamera()->SetPosition(jsonProfileData.defaultCameraPosition);
            renderer->GetActiveCamera()->SetViewUp(jsonProfileData.defaultViewUpVector);
            profileInput.close();
        }
    }
    renderer->ResetCamera();
    if (newConfigCreated) {     // camera default -> camera values -> struct
        renderer->GetActiveCamera()->GetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);
        cameraViewAngle = renderer->GetActiveCamera()->GetViewAngle();
        renderer->GetActiveCamera()->GetWindowCenter(cameraWindowCenter);
        cameraFocalDistance = renderer->GetActiveCamera()->GetDistance();
        
        jsonConfigData.clippingRange[0] = cameraClippingRangeNear;
        jsonConfigData.clippingRange[1] = cameraClippingRangeFar;
        jsonConfigData.viewAngle = cameraViewAngle;
        jsonConfigData.windowCenter[0] = cameraWindowCenter[0];
        jsonConfigData.windowCenter[1] = cameraWindowCenter[1];
        jsonConfigData.focalDistance = cameraFocalDistance;
    }
    else {      // struct -> camera values -> camera
        cameraClippingRangeNear = jsonConfigData.clippingRange[0];
        cameraClippingRangeFar = jsonConfigData.clippingRange[1];
        cameraViewAngle = jsonConfigData.viewAngle;
        cameraWindowCenter[0] = jsonConfigData.windowCenter[0];
        cameraWindowCenter[1] = jsonConfigData.windowCenter[1];
        cameraFocalDistance = jsonConfigData.focalDistance;

        renderer->GetActiveCamera()->SetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);
        renderer->GetActiveCamera()->SetViewAngle(cameraViewAngle);
        renderer->GetActiveCamera()->SetWindowCenter(cameraWindowCenter[0], cameraWindowCenter[1]);
        renderer->GetActiveCamera()->SetDistance(cameraFocalDistance);
    }
}

void saveCameraSettingsToJSON(string path, saveJSONMode mode) {
    json j;
    if (mode == config) {
        double tempPosition[3], tempUpVector[3];
        renderer->GetActiveCamera()->GetPosition(tempPosition);
        renderer->GetActiveCamera()->GetViewUp(tempUpVector);
        for (int i = 0; i < 3; i++) { 
            if (tempPosition[i] != jsonProfileData.defaultCameraPosition[i])     jsonConfigData.overrideEnable = true;
            if (tempUpVector[i] != jsonProfileData.defaultViewUpVector[i])     jsonConfigData.overrideEnable = true;
        }
        if (jsonConfigData.overrideEnable) {
            renderer->GetActiveCamera()->GetPosition(jsonConfigData.overrideCameraPosition);
            renderer->GetActiveCamera()->GetViewUp(jsonConfigData.overrideViewUp);
        }
        renderer->GetActiveCamera()->GetClippingRange(jsonConfigData.clippingRange);
        jsonConfigData.viewAngle = renderer->GetActiveCamera()->GetViewAngle();
        renderer->GetActiveCamera()->GetWindowCenter(jsonConfigData.windowCenter);
        jsonConfigData.focalDistance = renderer->GetActiveCamera()->GetDistance();
        j = jsonConfigData;
        ofstream file(finalConfigFileName);
        file << j;
        file.close();
    }
    else {
        if (jsonConfigData.source != "Online") {
            renderer->GetActiveCamera()->GetPosition(jsonProfileData.defaultCameraPosition);
            renderer->GetActiveCamera()->GetViewUp(jsonProfileData.defaultViewUpVector);
        }
        else {      // There is no point to apply default value to sources downloaded from online
            jsonProfileData.defaultCameraPosition[0] = 1.0;
            jsonProfileData.defaultCameraPosition[1] = 0.0;
            jsonProfileData.defaultCameraPosition[2] = 0.0;
            jsonProfileData.defaultViewUpVector[0] = 0.0;
            jsonProfileData.defaultViewUpVector[1] = 0.0;
            jsonProfileData.defaultViewUpVector[2] = 1.0;
        }
        j = jsonProfileData;
        ofstream file(finalProfileName);
        file << j;
        file.close();
    }
    
}

void configHandler() {
    if (mediaPath.isFile) {
        configFileName = mediaPath.directory + mediaPath.fileName + "meshed.json";
        ifstream configMeshed(configFileName);
        if (!configMeshed.is_open()) {
            configFileName = mediaPath.directory + mediaPath.fileName + ".json";
            ifstream config(configFileName);
            if (!config.is_open()) {
                cout << "No media config file found. " << endl;
                initializeConfig();
            }
            else {
                json j = json::parse(config);
                jsonConfigData = j.get<jsonHandler::mediaConfig>();
                finalConfigFileName = configFileName;
                finalReaderPath = mediaPath.directory + mediaPath.fileName + ".ply";
                config.close();
            }
        }
        else {
            json j = json::parse(configMeshed);
            jsonConfigData = j.get<jsonHandler::mediaConfig>();
            finalConfigFileName = configFileName;
            finalReaderPath = mediaPath.directory + mediaPath.fileName + "meshed.ply";
            configMeshed.close();
        }
    }
    else {
        configFileName = mediaPath.directory + "videoConfig.json";
        finalConfigFileName = configFileName;
        ifstream config(configFileName);
        if (!config.is_open()) {
            cout << "No media config file found. " << endl;
            initializeConfig();
        }
        else {
            json j = json::parse(config);
            jsonConfigData = j.get<jsonHandler::mediaConfig>();
            if (jsonConfigData.isSrcHasFaceEmbed)   finalReaderPath = videoFileList[0];
            else {
                string destDir = mediaPath.directory + "meshed";
#ifdef osWindows
                destDir += "\\";
#elif osMacOS
                destDir += "/";
#elif osLinux
                destDir += "/";
#endif
                // rescan for meshed
                videoFileList = Util::scanFileByType(destDir.c_str(), ".ply");
                finalReaderPath = videoFileList[0];
            }
            config.close();
        }
    }
}

void something::vtkTimerCallback2::Execute(vtkObject* caller, unsigned long eventId,
    void* vtkNotUsed(callData)) {
    vtkRenderWindowInteractor* iren =
        dynamic_cast<vtkRenderWindowInteractor*>(caller);
    if (vtkCommand::TimerEvent == eventId)
    {
        ++this->TimerCount;
    }
    if (TimerCount < videoFileList.size())
    {
        previousActor = allActors->GetLastActor();
        previousActor->VisibilityOff();
        nextActor = allActors->GetNextActor(actorIndex);
        nextActor->VisibilityOn();
    }
    else
    {
        iren->DestroyTimer();
    }
}

void vtkHandler::pclInterface(vtkPolyData* inputData) {
    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputData(inputData);
    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<something::KeyPressInteractorStyle> style;
    style->renderer = renderer;
    style->renderWindow = renderWindow;
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);
    renderWindow->Render();
    iren->Start();
    renderWindow->Finalize();
}

void threadProcedure(int i, int j, promise<vtkActor*> promiseRes) {
    vtkNew<vtkPLYReader> readerVecMember;
    vtkNew<vtkPolyDataMapper> mapperVecMember;
    vtkNew<vtkActor> actorVecMember;
    readerVecMember->SetFileName(videoFileList[i + j].c_str());
    readerVecMember->Update();
    mapperVecMember->SetInputConnection(readerVecMember->GetOutputPort());
    actorVecMember->SetMapper(mapperVecMember);
    actorVecMember->VisibilityOff();
    promiseRes.set_value(actorVecMember);
}

void multithreadRequest() {
    vector<vtkActor*> actorVec;
    vector<thread> threadList;
    vector<future<vtkActor*>> futureVec;
    vtkNew<vtkActorCollection> allActors;
    int progressPreviousPercent = 0;
    int percent = 0, progress = 0;
    progressbar bar(100);
    bar.reset();
    bar.update();       // initial 0%
    for (int i = 0; i < videoFileList.size(); i += actualThreads) {
        int threadCount = actualThreads;
        int temp = videoFileList.size() - 1 - i;
        if (temp < actualThreads)    threadCount = temp;
        
        for (int j = 0; j < threadCount; j++) {
            promise<vtkActor*> promise;
            futureVec.push_back(promise.get_future());
            threadList.push_back(thread(threadProcedure, i, j, move(promise)));
        }

        for (std::thread& th : threadList) {
            th.join();
            progress++;
            // progress bar
            percent = progress * 100 / videoFileList.size();
            if (percent > progressPreviousPercent) {
                bar.update();
                progressPreviousPercent = percent;  // update
            }
        }
        for (int j = 0; j < threadCount; j++)       actorVec.push_back(futureVec[j].get());
        for (int j = 0; j < threadCount; j++)       allActors->AddItem(actorVec[j]);
        
        futureVec.clear();
        threadList.clear();
    }
    cout << endl;       // give a newline when bar.update reaches 100

}

void backupCodeDownsampleMesh(char**);

void vtkHandler::dummyExperiment(char* argv[]) {
    backupCodeDownsampleMesh(argv);
}


void backupCodeDownsampleMesh(char* argv[]) {
    Util::pathResult temp = Util::pathParser(argv[1], ".ply");
    string destFile = temp.directory + temp.fileName + "meshed.ply";

    vtkNew<vtkPLYReader> localReader;
    localReader->SetFileName(argv[1]);
    localReader->Update();
    cout << "Original File Size: " << Util::getFileSize(argv[1]) << " Bytes. " << endl;
    cout << "Original File Polygon Num: " << localReader->GetOutput()->GetNumberOfPolys() << endl;
    cout << "Original File Point Num: " << localReader->GetOutput()->GetNumberOfPoints() << endl;
    vtkNew<vtkDelaunay2D> meshAlgo;
    meshAlgo->SetInputData(localReader->GetOutput());
    meshAlgo->SetAlpha(0.9);
    meshAlgo->Update();
    vtkNew<vtkPLYWriter> localWriter;
    localWriter->SetFileName(destFile.c_str());
    if (Util::findInPLY(argv[1], Util::alpha))   localWriter->SetArrayName("RGBA");
    else localWriter->SetArrayName("RGB");
    localWriter->SetInputConnection(meshAlgo->GetOutputPort());
    localWriter->Write();

    localReader->SetFileName(destFile.c_str());
    localReader->Update();
    cout << "Meshed File Size: " << Util::getFileSize(destFile.c_str()) << " Bytes. " << endl;
    cout << "Meshed File Polygon Num: " << localReader->GetOutput()->GetNumberOfPolys() << endl;
    cout << "Meshed File Point Num: " << localReader->GetOutput()->GetNumberOfPoints() << endl;

    string destFile2 = temp.directory + temp.fileName + "downsampledMeshed.ply";
    localReader->SetFileName(argv[1]);
    localReader->Update();
    vtkNew<vtkCleanPolyData> cleanPolyData;
    cleanPolyData->SetInputData(localReader->GetOutput());
    cleanPolyData->SetTolerance(0.01);
    cleanPolyData->Update();
    cout << "Downsampled Point Num: " << cleanPolyData->GetOutput()->GetNumberOfPoints() << endl;
    //vtkNew<vtkDelaunay2D> meshAlgo;
    meshAlgo->SetInputData(cleanPolyData->GetOutput());
    meshAlgo->SetAlpha(0.5);
    meshAlgo->Update();
    //vtkNew<vtkPLYWriter> localWriter;
    localWriter->SetFileName(destFile2.c_str());
    if (Util::findInPLY(argv[1], Util::alpha))   localWriter->SetArrayName("RGBA");
    else localWriter->SetArrayName("RGB");
    localWriter->SetInputConnection(meshAlgo->GetOutputPort());
    localWriter->Write();

    localReader->SetFileName(destFile2.c_str());
    localReader->Update();
    cout << "Downsampled Meshed File Size: " << Util::getFileSize(destFile2.c_str()) << " Bytes. " << endl;
    cout << "Downsampled Meshed File Polygon Num: " << localReader->GetOutput()->GetNumberOfPolys() << endl;
    cout << "Downsampled Meshed File Point Num: " << localReader->GetOutput()->GetNumberOfPoints() << endl;
}

/*
void threadOp(int i, vtkActor* newActor) {
    vtkNew<vtkPLYReader> readerVecMember;
    vtkNew<vtkPolyDataMapper> mapperVecMember;
    vtkNew<vtkActor> actorVecMember;
    readerVecMember->SetFileName(videoFileList[i].c_str());
    readerVecMember->Update();
    mapperVecMember->SetInputConnection(readerVecMember->GetOutputPort());
    actorVecMember->SetMapper(mapperVecMember);
    actorVecMember->VisibilityOff();
    newActor = actorVecMember;
}

void vtkHandler::dummyExperiment(const char* path) {
    vtkNew<vtkActor> actor1;
    vtkNew<vtkActor> actor2;
    vtkNew<vtkActor> actor3;
    vtkNew<vtkActor> actor4;
    vtkNew<vtkActor> actor5;
    vtkNew<vtkActor> actor6;
    vtkNew<vtkActor> actor7;
    vtkNew<vtkActor> actor8;
    //vector<vtkActor*> actors;
    vector<thread> threadList;

    int progressPreviousPercent = 0;
    int percent = 0, progress = 0;
    progressbar bar(100);
    bar.reset();
    bar.update();       // initial 0%
    for (int i = 0; i < videoFileList.size(); i += actualThreads) {
        int threadCount = actualThreads;
        int temp = videoFileList.size() - 1 - i;
        if (temp < actualThreads)    threadCount = temp;
        //actors.resize(threadCount);
        //for (int j = 0; j < threadCount; j++) {
            thread th1(threadOp, i, actor1);
            thread th2(threadOp, i, actor2);
            thread th3(threadOp, i, actor3);
            thread th4(threadOp, i, actor4);
            thread th5(threadOp, i, actor5);
            thread th6(threadOp, i, actor6);
            thread th7(threadOp, i, actor7);
            thread th8(threadOp, i, actor8);
        //}

        //for (std::thread& th : threadList) {
            th1.join();
            th2.join();
            th3.join();
            th4.join();
            th5.join();
            th6.join();
            th7.join();
            th8.join();
            progress += 8;
            // progress bar
            percent = progress * 100 / videoFileList.size();
            if (percent > progressPreviousPercent) {
                bar.update();
                progressPreviousPercent = percent;  // update
            }
        //}
        //for (int j = 0; j < threadCount; j++)       actorVec.push_back(futureVec[j].get());
        //for (int j = 0; j < threadCount; j++)       allActors->AddItem(actors[j]);
        allActors->AddItem(actor1);
        allActors->AddItem(actor2);
        allActors->AddItem(actor3);
        allActors->AddItem(actor4);
        allActors->AddItem(actor5);
        allActors->AddItem(actor6);
        allActors->AddItem(actor7);
        allActors->AddItem(actor8);

        //futureVec.clear();
        threadList.clear();
    }
    cout << endl;       // give a newline when bar.update reaches 100


}
*/