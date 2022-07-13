#include "vtkActor.h"
//#include "vtkActorCollection.h"
#include "vtkCamera.h"
#include "vtkLookingGlassInterface.h"
#include "vtkLookingGlassPass.h"
#include "vtkNew.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPLYReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
//#include "vtkRegressionTestImage.h"
#include "vtkRenderStepsPass.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderer.h"
//#include "vtkTestUtilities.h"
#include "vtkTextureObject.h"
#include "vtkAnimationScene.h"

#include "vtkCompositeDataGeometryFilter.h"
#include "vtkCompositeDataPipeline.h"
#include "vtkContourFilter.h"
#include "vtkInformation.h"
#include "vtkSmartPointer.h"
#include "vtkTemporalFractal.h"
#include "vtkTemporalInterpolator.h"
#include "vtkTemporalShiftScale.h"
#include "vtkThreshold.h"

#include "stdio.h"
//#include "conio.h"  // For getch()

#include "filesystem"
#include "vector"

//#include "vld.h"

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
std::vector<std::string> fileList;
vtkNew<vtkRenderer> renderer;
vtkNew<vtkRenderWindow> renderWindow;

namespace fs = std::filesystem;

namespace {

    // Define interaction style
    class KeyPressInteractorStyle : public vtkInteractorStyleTrackballCamera
    {
    public:
        static KeyPressInteractorStyle* New();
        vtkTypeMacro(KeyPressInteractorStyle, vtkInteractorStyleTrackballCamera);

        virtual void OnKeyPress() override
        {
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

            renderer->GetActiveCamera()->Azimuth(cameraHorizontalAngle);
            renderer->GetActiveCamera()->Elevation(cameraVerticalAngle);
            renderer->GetActiveCamera()->OrthogonalizeViewUp();
            renderer->GetActiveCamera()->Zoom(cameraScale);
            renderWindow->Render();

            cameraVerticalAngle = 0.0;
            cameraHorizontalAngle = 0.0;
            cameraScale = 1.0;

            // Forward events
            vtkInteractorStyleTrackballCamera::OnKeyPress();
        }
    };
    vtkStandardNewMacro(KeyPressInteractorStyle);
}

//------------------------------------------------------------------------------
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
    renderer->SetBackground(0.3, 0.4, 0.6);
    renderWindow->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);
    vtkNew<KeyPressInteractorStyle> style;
    iren->SetInteractorStyle(style);
    style->SetCurrentRenderer(renderer);

    char* fileName = path;
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName);
    reader->Update();

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(reader->GetOutputPort());

    vtkNew<vtkActor> actor;
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
     
}

// Reference: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
void scanFileByType(char* path, const char* fileType) {
    for (const auto& entry : fs::directory_iterator(path)) {
        fs::path temp0 = entry.path();
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
    //printf("Shrinking\n");
    fileList.shrink_to_fit();
}