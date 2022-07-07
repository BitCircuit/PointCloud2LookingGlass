#include "vtkActor.h"
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
#include "vtkRenderer.h"
#include "vtkTestUtilities.h"
#include "vtkTextureObject.h"
#include "stdio.h"
#include "conio.h"

//#include "vld.h"

// Keyboard value (US)
#define arrowUp     38
#define arrowDown   40
#define arrowLeft   37
#define arrowRight  39
#define KEY_ESC     27

// Some variables
double cameraHorizontalAngle = 0.0, cameraVerticalAngle = 0.0, cameraScale = 1.0;

//------------------------------------------------------------------------------
int vtkHandler(int argc, char* argv[])
{
    vtkNew<vtkRenderer> renderer;
    renderer->DebugOn();
    renderer->SetBackground(0.3, 0.4, 0.6);
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);

    char* fileName = argv[2];// vtkTestUtilities::ExpandDataFileName(argc, argv, "Data/dragon.ply");
    vtkNew<vtkPLYReader> reader;
    reader->SetFileName(fileName);
    reader->Update();

    //delete[] fileName;

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
    while (1) {
        renderer->GetActiveCamera()->Azimuth(cameraHorizontalAngle);
        renderer->GetActiveCamera()->Elevation(cameraVerticalAngle);
        renderer->GetActiveCamera()->OrthogonalizeViewUp();
        renderer->GetActiveCamera()->Zoom(cameraScale);
        //renderWindow->PrintSelf();
        renderWindow->Render();

        // Reset values after they are used
        cameraVerticalAngle = 0.0;
        cameraHorizontalAngle = 0.0;
        cameraScale = 1.0;

        switch (char operation = getch()) {
        case 'w':
            cameraVerticalAngle += 5;
            printf("Turning up. %f\n", cameraVerticalAngle);
            break;
        case 's':
            cameraVerticalAngle -= 5;
            printf("Turning down. %f\n", cameraVerticalAngle);
            break;
        case 'a':
            cameraHorizontalAngle -= 5;
            printf("Turning left. %f\n", cameraHorizontalAngle);
            break;
        case 'd':
            cameraHorizontalAngle += 5;
            printf("Turning right. %f\n", cameraHorizontalAngle);
            break;
        case 'e':
            cameraScale += 0.5;
            printf("Zooming In. %f\n", cameraScale);
            break;
        case 'q':
            cameraScale -= 0.5;
            printf("Zooming Out. %f\n", cameraScale);
            break;
        case 'r':
            cameraVerticalAngle = 0.0;
            cameraHorizontalAngle = 0.0;
            cameraScale = 1.0;
            printf("Resetting Camera. \n");
            renderer->ResetCamera();
            break;
        default:
            goto exit_loop;
        }
    }
exit_loop:
    printf("Job is done. \n");
    return 0;
}