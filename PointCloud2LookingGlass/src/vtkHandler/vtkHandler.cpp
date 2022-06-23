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

//------------------------------------------------------------------------------
int vtkHandler(int argc, char* argv[])
{
    vtkNew<vtkRenderer> renderer;
    renderer->SetBackground(0.3, 0.4, 0.6);
    vtkNew<vtkRenderWindow> renderWindow;
    renderWindow->AddRenderer(renderer);
    vtkNew<vtkRenderWindowInteractor> iren;
    iren->SetRenderWindow(renderWindow);

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

    renderer->GetActiveCamera()->SetPosition(0, 0, 1);
    renderer->GetActiveCamera()->SetFocalPoint(0, 0, 0);
    renderer->GetActiveCamera()->SetViewUp(0, 1, 0);
    renderer->ResetCamera();
    renderer->GetActiveCamera()->Azimuth(45.0);
    //renderer->GetActiveCamera()->SetRoll(45.0);
    //renderer->GetActiveCamera()->SetParallelScale(10);
    renderer->GetActiveCamera()->Zoom(1.0);
    renderWindow->Render();

    int retVal = vtkRegressionTestImageThreshold(renderWindow, 15);
    if (retVal == vtkRegressionTester::DO_INTERACTOR)
    {
        iren->Start();
    }
    char dummy[2];
    printf("Press any key to exit...");
    scanf("%1s", dummy);
    return !retVal;
}