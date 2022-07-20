#include "vtkHandler.h"

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

using namespace std;

// Some enum
enum cameraClippingPlaneSelection { none, near, far };
enum singlePLYOperationModes {normal, calibration};

// Some functions
void singlePLYPlot(char*);
void dummyExperiment(char*);
void showVideo(char*, int);
void plyMesh(char*, char*);

// Some variables
double cameraClippingRangeNear = 0.0, cameraClippingRangeFar = 0.0;
singlePLYOperationModes singlePLYOperationMode = normal;

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
        cameraClippingPlaneSelection camClipSel = none;
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

void vtkHandler(char* path) {
    int len = strlen(path);
    const char* lastFourChar = &path[len - 4];
    if (strncmp(lastFourChar, ".ply", 4) == 0)  singlePLYPlot(path);    // if path ends with .ply
    else showVideo(path, 30);       // if path ends without .ply -> directory
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
    renderer->GetActiveCamera()->GetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);

    renderWindow->Render();
    iren->Start();
}

void showVideo(char* path, int FPS) {

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

    renderWindow->Render();

    
    scene->SetModeToSequence();
    scene->SetFrameRate(FPS);
    scene->SetStartTime(0);
    scene->SetEndTime(fileList[0].size() / FPS);

    for (int frameIndex = 0; frameIndex < fileList[0].size(); frameIndex++) {
        vtkNew<something::videoFrames> frame;
        frame->filePath = fileList[frameIndex];
        frame->reader = reader;
        frame->renderWindow = renderWindow;
        frame->SetTimeModeToNormalized();
        frame->SetStartTime((double)(frameIndex / fileList[0].size()));
        frame->SetEndTime((double)((frameIndex + 1) / fileList[0].size()));
        scene->AddCue(frame);
        //printf("Adding frame %d from %s\n", frameIndex+1, frame->filePath.c_str());
    }

    scene->Play();
    scene->Stop();
    //iren->Start();
}

void something::KeyPressInteractorStyle::OnKeyPress() {
    // Get the keypress
    vtkRenderWindowInteractor* rwi = this->Interactor;
    std::string key = rwi->GetKeySym();

    // Handle an arrow key
    if (key == "Up") {
        renderer->GetActiveCamera()->Elevation(5);
    }
    if (key == "Down") {
        renderer->GetActiveCamera()->Elevation(-5);
    }
    if (key == "Left") {
        renderer->GetActiveCamera()->Azimuth(-5);
    }
    if (key == "Right") {
        renderer->GetActiveCamera()->Azimuth(5);
    }

    // Handle a "normal" key
    if (key == "i") {
        renderer->GetActiveCamera()->Zoom(1.5);
        renderer->GetActiveCamera()->OrthogonalizeViewUp();
    }
    if (key == "o") {
        renderer->GetActiveCamera()->Zoom(0.5);
        renderer->GetActiveCamera()->OrthogonalizeViewUp();
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
        renderer->GetActiveCamera()->SetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);
    }
    if (key == "p") {
        if (camClipSel == none)     printf("No plane selected. \n");
        else if (camClipSel == near)    cameraClippingRangeNear += 0.5;
        else if (camClipSel == far)     cameraClippingRangeFar += 0.5;
        renderer->GetActiveCamera()->SetClippingRange(cameraClippingRangeNear, cameraClippingRangeFar);
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

    renderWindow->Render();

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

void dummyExperiment(char* plyPath) {
    
}