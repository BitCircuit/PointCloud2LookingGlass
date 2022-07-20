#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkLookingGlassInterface.h"
#include "vtkLookingGlassPass.h"
#include "vtkNew.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPLYReader.h"
#include "vtkPLYWriter.h"
#include "vtkJPEGReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRenderStepsPass.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderer.h"
#include "vtkTextureObject.h"
#include "vtkAnimationScene.h"
#include "vtkSmartPointer.h"

#include "vtkDelaunay2D.h"

#include "stdio.h"

#include "filesystem"
#include "vector"

#include "../progressbar.hpp"

using namespace std;

// Some enum
enum cameraClippingPlaneSelection { none, near, far };
//enum fileScanTypes { config, profile, depth, colorJPG };
enum singlePLYOperationModes {normal, calibration};
enum findMode {alpha, color};

// Some functions
void singlePLYPlot(char*);
void dummyExperiment(char*);
void showVideo(char*, int);
void scanFile(char*);
void plyMesh(char*, char*);
bool findInPLY(char*, findMode);
//int TestTemporalFractal();

// Some variables
double cameraHorizontalAngle = 0.0, cameraVerticalAngle = 0.0, cameraScale = 1.0;
double cameraClippingRangeNear = 0.0, cameraClippingRangeFar = 0.0;
cameraClippingPlaneSelection camClipSel = none;
singlePLYOperationModes singlePLYOperationMode = normal;

vector<string> videoDepthFileList;
vector<string> videoColorFileList;
vector<string> configFilePath;
vector<string> cameraProfilePath;
int numDepthFileScanned = 0, numColorFileScanned = 0;
bool isDepthFileFound = false, isColorFileFound = false, isConfigFileFound = false, isCameraProfileFound = false;

progressbar bar(100);


namespace something{
    // Define interaction style
    class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera {
    public:
        static KeyPressInteractorStyle* New();
        vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);
        vtkRenderer* renderer;
        vtkRenderWindow* renderWindow;
        bool videoMode = false;
        bool isVideoPlaying = false;
        vtkAnimationScene* scene;
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
}

void vtkHandler(char* argv[]) {
    if (strcmp(argv[2], "-p") == 0) {
        singlePLYPlot(argv[3]);
    }
    else if (strcmp(argv[2], "-m") == 0) {
        showVideo(argv[3], 30);
    }
    else if (strcmp(argv[2], "-t") == 0) {
        //TestTemporalFractal();
    }
    else if (strcmp(argv[2], "-s") == 0) {
        scanFile(argv[3]);
    }
    else if (strcmp(argv[2], "-d") == 0) {
        dummyExperiment(argv[3]);
    }
    else {
        printf("Unrecognized Commend. \n");
    }
}

void singlePLYPlot(char* path) {
    if (strlen(path) > 100) {
        cout << "Input Path too long. \n" << endl;
        return;
    }

    singlePLYOperationMode = normal;
    
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<something::KeyPressInteractorStyle> style;
    vtkNew<vtkPLYReader> reader;
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;

    style->renderer = renderer;
    style->renderWindow = renderWindow;
    style->videoMode = false;

    renderer->SetBackground(0.3, 0.4, 0.6);
    renderWindow->AddRenderer(renderer);
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);

    reader->SetFileName(path);
    reader->Update();
    if (reader->GetOutput()->GetNumberOfPolys() == 0) {
        int i;
        for (i = strlen(path); i > 0; i--) {
            if (path[i - 1] == '\\' || path[i - 1] == '/') {
                break;
            }
        }
        //string currentFile[2];
        char fileCurrentPath[100] = "asd";  // dummy initialization
        char fileCurrentName[100] = "asd";
        strncpy(fileCurrentPath, path, i);
        fileCurrentPath[i] = '\0';
        //cout << "The file path: " << fileCurrentPath << endl;
        int x;
        for (x = i; path[x] != '.'; x++)  fileCurrentName[x - i] = path[x];
        fileCurrentName[x - i + 1] = '\0';
        //cout << "The file name: " << fileCurrentName << endl;
        char destFile[100] = "asd";
        strncpy(destFile, path, i);
        destFile[i] = '\0';
        strcat(destFile, fileCurrentName);
        strcat(destFile, "meshed.ply");
        plyMesh(path, destFile);
        reader->SetFileName(destFile);
        reader->Update();
    }

    mapper->SetInputConnection(reader->GetOutputPort());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);

    // For anti-alising
    renderWindow->SetMultiSamples(8);

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

    renderer->GetActiveCamera()->SetPosition(1, 0, 0);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(cameraHorizontalAngle);
    renderer->GetActiveCamera()->Elevation(cameraVerticalAngle);
    renderer->GetActiveCamera()->Zoom(cameraScale);
    renderer->GetActiveCamera()->GetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);

    renderWindow->Render();
    iren->Start();
}

void showVideo(char* path, int FPS) {
    scanFile(path);

    if (!isConfigFileFound) {
        singlePLYOperationMode = calibration;
    }

    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<something::KeyPressInteractorStyle> style;
    vtkNew<vtkPLYReader> reader;
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;

    // Create an Animation Scene
    vtkAnimationScene* scene = vtkAnimationScene::New();

    style->renderer = renderer;
    style->renderWindow = renderWindow;
    style->videoMode = true;
    style->scene = scene;

    renderWindow->AddRenderer(renderer);
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);

    reader->SetFileName(videoDepthFileList[0].c_str());
    reader->Update();

    mapper->SetInputConnection(reader->GetOutputPort());
    actor->SetMapper(mapper);
    renderer->AddActor(actor);

    // For anti-alising
    renderWindow->SetMultiSamples(0);

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

    renderer->GetActiveCamera()->SetPosition(1, 0, 0);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(cameraHorizontalAngle);
    renderer->GetActiveCamera()->Elevation(cameraVerticalAngle);
    renderer->GetActiveCamera()->Zoom(cameraScale);

    renderWindow->Render();

    
    scene->SetModeToSequence();
    scene->SetFrameRate(FPS);
    scene->SetStartTime(0);
    scene->SetEndTime(numDepthFileScanned / FPS);

    for (int frameIndex = 0; frameIndex < numDepthFileScanned; frameIndex++) {
        vtkNew<something::videoFrames> frame;
        frame->filePath = videoDepthFileList[frameIndex];
        frame->reader = reader;
        frame->renderWindow = renderWindow;
        frame->SetTimeModeToNormalized();
        frame->SetStartTime((double)(frameIndex / numDepthFileScanned));
        frame->SetEndTime((double)((frameIndex + 1) / numDepthFileScanned));
        scene->AddCue(frame);
        //printf("Adding frame %d from %s\n", frameIndex+1, frame->filePath.c_str());
    }

    scene->Play();
    scene->Stop();
    //iren->Start();
}

// Reference: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
void scanFile(char* path) {
    for (const auto& entry : filesystem::directory_iterator(path)) {
        filesystem::path temp0 = entry.path();
        std::string temp{temp0.u8string()};
        const char* filePath = temp.c_str();
        int len = strlen(filePath);
        // Reference: https://stackoverflow.com/questions/5297248/how-to-compare-last-n-characters-of-a-string-to-another-string-in-c
        const char* lastFourChar = &filePath[len - 4];

        if (strcmp(lastFourChar, ".ply") == 0) {
            isDepthFileFound = true;
            numDepthFileScanned++;
            videoDepthFileList.push_back(filePath);
        }
        if (strcmp(lastFourChar, ".jpg") == 0) {
            isColorFileFound = true;
            numColorFileScanned++;
            videoColorFileList.push_back(filePath);
        }
        if (strcmp(lastFourChar, "json") == 0) {
            const char* jsonFileName = &filePath[len - 16];
            if (strcmp(jsonFileName, "cameraModel.json") == 0) {
                isCameraProfileFound = true;
                cameraProfilePath.push_back(filePath);
            }
            if (strcmp(jsonFileName, "videoConfig.json") == 0) {
                isConfigFileFound = true;
                configFilePath.push_back(filePath);
            }
        }
    }
    videoDepthFileList.shrink_to_fit();
    videoColorFileList.shrink_to_fit();
    cameraProfilePath.shrink_to_fit();
    configFilePath.shrink_to_fit();
}

void something::KeyPressInteractorStyle::OnKeyPress() {
    // Get the keypress
    vtkRenderWindowInteractor* rwi = this->Interactor;
    std::string key = rwi->GetKeySym();

    // Handle an arrow key
    if (key == "Up") {
        cameraVerticalAngle += 5;
    }
    if (key == "Down") {
        cameraVerticalAngle -= 5;
    }
    if (key == "Left") {
        cameraHorizontalAngle -= 5;
    }
    if (key == "Right") {
        cameraHorizontalAngle += 5;
    }

    // Handle a "normal" key
    if (key == "i") {
        cameraScale += 0.5;
    }
    if (key == "o") {
        cameraScale -= 0.5;
    }
    if (key == "a") {
        renderer->ResetCamera();
    }
    if (key == "r") {
        renderer->GetActiveCamera()->SetPosition(1, 0, 0);
        renderer->GetActiveCamera()->SetViewUp(0, 0, 1);
        // TODO: Add zooming restore
        renderer->ResetCamera();
    }
    if (key == "l") {
        if (camClipSel == none)     printf("No plane selected. \n");
        else if (camClipSel == near)    cameraClippingRangeNear -= 0.5;
        else if (camClipSel == far)     cameraClippingRangeFar -= 0.5;
    }
    if (key == "p") {
        if (camClipSel == none)     printf("No plane selected. \n");
        else if (camClipSel == near)    cameraClippingRangeNear += 0.5;
        else if (camClipSel == far)     cameraClippingRangeFar += 0.5;
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
            camClipSel = none;
            printf("Camera Clipping Plane Unselected.\n");
            break;
        default:
            camClipSel = none;
            printf("Camera Clipping Plane Unselected.\n");
        }
    }
    if (key == " ") {
        isVideoPlaying = !isVideoPlaying;
    }

    renderer->GetActiveCamera()->Azimuth(cameraHorizontalAngle);
    renderer->GetActiveCamera()->Elevation(cameraVerticalAngle);
    renderer->GetActiveCamera()->OrthogonalizeViewUp();
    renderer->GetActiveCamera()->Zoom(cameraScale);
    renderer->GetActiveCamera()->SetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);
    renderWindow->Render();

    cameraVerticalAngle = 0.0;
    cameraHorizontalAngle = 0.0;
    cameraScale = 1.0;

    if (videoMode) {
        if (isVideoPlaying)  scene->Play();
        else  scene->Stop();
    }

    // Forward events
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void something::videoFrames::TickInternal(double currenttime, double deltatime, double clocktime) {
    reader->SetFileName(this->filePath.c_str());
    reader->Update();
    renderWindow->Render();
}

// It may lose normal/vector information
void plyMesh(char* srcFile, char* destPathAndFileName) {
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(srcFile);
    reader->Update();
    vtkNew<vtkDelaunay2D> meshAlgo;
    meshAlgo->SetInputData(reader->GetOutput());
    meshAlgo->SetAlpha(0.075);
    meshAlgo->Update();
    vtkNew<vtkPLYWriter> writer;
    writer->SetFileName(destPathAndFileName);
    if (findInPLY(srcFile, alpha))   writer->SetArrayName("RGBA");
    else writer->SetArrayName("RGB");
    writer->SetInputConnection(meshAlgo->GetOutputPort());
    writer->Write();
}

bool findInPLY(char* path, findMode type) {
    bool result = false;
    FILE* src = fopen(path, "rb+");
    if (src == NULL) {
        printf("File not exist. \n");
        return false;
    }
    char* info;
    info = (char*)malloc(100 * sizeof(char));
    while (true) {
        fgets(info, 100, src);      // Read until end of line or EOF, max reading char is set to 100
        if (strncmp(info, "end_header", 10) == 0)   break;
        if (type == alpha && strncmp(info, "property uchar alpha", 20) == 0) {
            result = true;
            break;
        }
        if (type == color && strncmp(info, "property uchar red", 18) == 0) {
            result = true;
            break;
        }
    }
    fclose(src);
    return result;
}

void dummyExperiment(char* plyPath) {
    
}