#include "vtkActor.h"
#include "vtkActorCollection.h"
#include "vtkCamera.h"
#include "vtkLookingGlassInterface.h"
#include "vtkLookingGlassPass.h"
#include "vtkNew.h"
#include "vtkOpenGLRenderer.h"
#include "vtkPLYReader.h"
#include "vtkPolyDataMapper.h"
#include "vtkProperty.h"
#include "vtkRegressionTestImage.h"
#include "vtkRenderStepsPass.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"
#include "vtkTextureObject.h"
#include "vtkAnimationScene.h"
#include "stdio.h"
#include "conio.h"

#include "filesystem"

//#include "vld.h"

// Some variables
void singlePLYPlot(char*);
void showVideo(char*);
double cameraHorizontalAngle = 0.0, cameraVerticalAngle = 0.0, cameraScale = 1.0;
vtkNew<vtkRenderer> renderer;
vtkNew<vtkRenderWindow> renderWindow;

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
        showVideo(argv[3]);
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

void showVideo(char* path) {
    
    
    
    renderWindow->AddRenderer(renderer);

}