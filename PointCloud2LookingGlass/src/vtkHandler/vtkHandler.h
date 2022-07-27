#ifndef VTKHANDLER_H
#define VTKHANDLER_H

#include "../Util/Util.h"
#include "../jsonHandler/jsonHandler.h"

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

namespace vtkHandler {

	void vtkHandler(char**);
	void dummyExperiment(const char*);
	void pclInterface(vtkPolyData*);
	
};

#endif // !VTKHANDLER_H