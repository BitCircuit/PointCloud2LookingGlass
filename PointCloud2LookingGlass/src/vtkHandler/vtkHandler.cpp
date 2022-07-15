#include "vtkActor.h"
#include "vtkCamera.h"
#include "vtkLookingGlassInterface.h"
#include "vtkLookingGlassPass.h"
#include "vtkNew.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPLYReader.h"
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

#include "stdio.h"

#include "filesystem"
#include "vector"

using namespace std;

// File types for scanFileByType function
#define plyFile     ".ply"
#define jpgFile     ".jpg"
#define pngFile     ".png"

// Some functions
void singlePLYPlot(char*);
void showVideo(char*, int);
void scanFileByType(char*, const char*);
//int TestTemporalFractal();

// Some variables
double cameraHorizontalAngle = 0.0, cameraVerticalAngle = 0.0, cameraScale = 1.0;
int numFileScanned = 0;
vector<string> fileList;

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
        scanFileByType(argv[3], plyFile);
    }
    else {
        printf("Unrecognized Commend. \n");
    }
}

void singlePLYPlot(char* path) {
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<something::KeyPressInteractorStyle> style;
    vtkNew<vtkPLYReader> reader;
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;

    style->renderer = renderer;
    style->renderWindow = renderWindow;

    renderer->SetBackground(0.3, 0.4, 0.6);
    renderWindow->AddRenderer(renderer);
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);

    char* fileName = path;
    reader->SetFileName(fileName);
    reader->Update();

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

    renderWindow->Render();
    iren->Start();
}

void showVideo(char* path, int FPS) {
    scanFileByType(path, plyFile);
    //for (auto i = fileList.begin(); i != fileList.end(); i++)   std::cout << *i << std::endl;
    //printf("%s\n",fileList[0].c_str());
    //printf("File Scan Done.\n");
    vtkNew<vtkRenderer> renderer;
    vtkNew<vtkRenderWindow> renderWindow;
    vtkNew<vtkRenderWindowInteractor> iren;
    vtkNew<something::KeyPressInteractorStyle> style;
    vtkNew<vtkPLYReader> reader;
    vtkNew<vtkPolyDataMapper> mapper;
    vtkNew<vtkActor> actor;

    // Create an Animation Scene
    vtkAnimationScene* scene = vtkAnimationScene::New();

    style->videoMode = true;
    style->scene = scene;

    renderWindow->AddRenderer(renderer);
    iren->SetRenderWindow(renderWindow);
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);

    reader->SetFileName(fileList[0].c_str());
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
    scene->SetEndTime(numFileScanned / FPS);

    for (int frameIndex = 0; frameIndex < numFileScanned; frameIndex++) {
        vtkNew<something::videoFrames> frame;
        frame->filePath = fileList[frameIndex];
        frame->reader = reader;
        frame->renderWindow = renderWindow;
        frame->SetTimeModeToNormalized();
        frame->SetStartTime((double)(frameIndex / numFileScanned));
        frame->SetEndTime((double)((frameIndex + 1) / numFileScanned));
        scene->AddCue(frame);
        //printf("Adding frame %d from %s\n", frameIndex+1, frame->filePath.c_str());
    }

    scene->Play();
    scene->Stop();
    //iren->Start();
}

// Reference: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
void scanFileByType(char* path, const char* fileType) {
    for (const auto& entry : filesystem::directory_iterator(path)) {
        filesystem::path temp0 = entry.path();
        std::string temp{temp0.u8string()};
        const char* filePath = temp.c_str();
        int len = strlen(filePath);
        // Reference: https://stackoverflow.com/questions/5297248/how-to-compare-last-n-characters-of-a-string-to-another-string-in-c
        const char* lastFourChar = &filePath[len - 4];
        if (strcmp(lastFourChar, fileType) == 0) {
            numFileScanned++;
            fileList.push_back(filePath);
        }
    }
    fileList.shrink_to_fit();
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
    if (key == " ") {
        isVideoPlaying = !isVideoPlaying;
    }

    renderer->GetActiveCamera()->Azimuth(cameraHorizontalAngle);
    renderer->GetActiveCamera()->Elevation(cameraVerticalAngle);
    renderer->GetActiveCamera()->OrthogonalizeViewUp();
    renderer->GetActiveCamera()->Zoom(cameraScale);
    renderWindow->Render();

    cameraVerticalAngle = 0.0;
    cameraHorizontalAngle = 0.0;
    cameraScale = 1.0;

    if (isVideoPlaying)  scene->Play();
    else  scene->Stop();

    // Forward events
    vtkInteractorStyleTrackballCamera::OnKeyPress();
}

void something::videoFrames::TickInternal(double currenttime, double deltatime, double clocktime) {
    reader->SetFileName(this->filePath.c_str());
    reader->Update();
    renderWindow->Render();
}