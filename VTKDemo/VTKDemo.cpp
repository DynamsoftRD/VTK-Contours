#include "vtkSmartPointer.h"

#include "vtkDEMReader.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkProperty.h"
#include "vtkCamera.h"
#include "vtkImageDataGeometryFilter.h"
#include "vtkWarpScalar.h"
#include "vtkPolyDataNormals.h"
#include "vtkImageData.h"
#include "vtkLookupTable.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkPolyDataCollection.h"
#include "vtkImageActor.h"
#include "vtkImageProperty.h"
#include "vtkMarchingSquares.h"
#include "vtkAppendPolyData.h"
#include "vtkImageResample.h"

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL);
VTK_MODULE_INIT(vtkInteractionStyle);

int main()
{
	//1. Read regular dataset (DEM).
	const char* fname = "SainteHelens.dem";

	vtkSmartPointer<vtkDEMReader> demReader = vtkSmartPointer<vtkDEMReader>::New();
	demReader->SetFileName(fname);
	demReader->Update();

	//1.1 Downsize Image Grids.
	vtkSmartPointer<vtkImageResample>  resample = vtkSmartPointer<vtkImageResample>::New();
	resample->SetInputConnection(demReader->GetOutputPort());
	resample->SetDimensionality(2);
	resample->SetAxisMagnificationFactor(0, 0.25);
	resample->SetAxisMagnificationFactor(1, 0.25);
	resample->Update();

	//1.2 Obtain min & max height value of DEM. 
	double lo = resample->GetOutput()->GetScalarRange()[0];
	double hi = resample->GetOutput()->GetScalarRange()[1];

	//2 Generate Contour
	vtkSmartPointer<vtkMarchingSquares> marchingSquares = vtkSmartPointer<vtkMarchingSquares>::New();
	marchingSquares->SetInputConnection(resample->GetOutputPort());
	marchingSquares->GenerateValues(20, lo, hi);
	//marchingSquares->SetNumberOfContours(4);
	//marchingSquares->SetValue(0, lo+(hi-lo)/5);
	//marchingSquares->SetValue(1, lo+(hi-lo)/5*2);
	//marchingSquares->SetValue(2, lo+(hi-lo)/5*3);
	//marchingSquares->SetValue(3, lo+(hi-lo)/5*4);
	marchingSquares->Update();

	//3. Extract geometry (vtkImageData->vtkPolyData)
	vtkSmartPointer<vtkImageDataGeometryFilter> surface = vtkSmartPointer<vtkImageDataGeometryFilter>::New();
	surface->SetInputConnection(resample->GetOutputPort());

	//4. Modify the z value to display height value of DEM
	vtkSmartPointer<vtkWarpScalar> warp = vtkSmartPointer<vtkWarpScalar>::New();
	warp->SetInputConnection(surface->GetOutputPort());
	warp->SetScaleFactor(3);	//z = height * scaleFactor
	warp->UseNormalOn();
	warp->SetNormal(0, 0, 1);
	warp->Update();

	//4.1 contour warp
	vtkSmartPointer<vtkWarpScalar> warp1 = vtkSmartPointer<vtkWarpScalar>::New();
	warp1->SetInputConnection(marchingSquares->GetOutputPort());
	warp1->SetScaleFactor(3);	//z = height * scaleFactor
	warp1->UseNormalOn();
	warp1->SetNormal(0, 0, 1);
	warp1->Update();

	//4.2 combine
	vtkSmartPointer<vtkAppendPolyData> append = vtkSmartPointer<vtkAppendPolyData>::New();
	append->AddInputData(warp->GetPolyDataOutput());
	append->AddInputData(warp1->GetPolyDataOutput());
	append->Update();

	//5. Mappers & Actors.

	//5.0 Set Colormap for different height.
	vtkSmartPointer<vtkLookupTable> lut = vtkSmartPointer<vtkLookupTable>::New();
	lut->SetHueRange(0.6, 0);
	lut->SetSaturationRange(1.0, 0);
	lut->SetValueRange(0.5, 1.0);

	//5.1 Grid Mode
	vtkSmartPointer<vtkPolyDataMapper> gridMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	gridMapper->SetInputConnection(surface->GetOutputPort());

	vtkSmartPointer<vtkActor> gridActor = vtkSmartPointer<vtkActor>::New();
	gridActor->SetMapper(gridMapper);
	gridActor->GetProperty()->SetRepresentationToWireframe();

	//5.2 Planar Image Mode
	vtkSmartPointer<vtkImageActor> imgActor = vtkSmartPointer<vtkImageActor>::New();
	imgActor->SetInputData(resample->GetOutput());

	imgActor->GetProperty()->SetColorLevel((lo + hi)*0.5);
	imgActor->GetProperty()->SetColorWindow(hi - lo);
	imgActor->GetProperty()->SetLookupTable(lut);

	//5.3 Coutour Mode
	vtkSmartPointer<vtkPolyDataMapper> contourMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	contourMapper->SetInputConnection(marchingSquares->GetOutputPort());
	contourMapper->SetScalarRange(lo, hi);
	contourMapper->SetLookupTable(lut);

	vtkSmartPointer<vtkActor> contourActor = vtkSmartPointer<vtkActor>::New();
	contourActor->SetMapper(contourMapper);

	//5.4 3D Mode
	//vtkPolyData *pd = warp->GetPolyDataOutput();
	vtkPolyData *pd = append->GetOutput();

	vtkSmartPointer<vtkPolyDataMapper> demMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	demMapper->SetInputData(pd);
	demMapper->SetScalarRange(lo, hi);
	demMapper->SetLookupTable(lut);

	vtkSmartPointer<vtkActor> demActor = vtkSmartPointer<vtkActor>::New();
	demActor->SetMapper(demMapper);
	demActor->GetProperty()->SetLineWidth(3);

	// 6. Renderer.
	//left,top (grid)
	vtkSmartPointer<vtkRenderer> ren1 = vtkSmartPointer<vtkRenderer>::New();
	ren1->SetViewport(0, 0.51, 0.49, 1);
	ren1->SetBackground(1, 1, 1);
	ren1->AddActor(gridActor);

	//right, top(image)
	vtkSmartPointer<vtkRenderer> ren2 = vtkSmartPointer<vtkRenderer>::New();
	ren2->SetViewport(0.51, 0.51, 1, 1);
	ren2->SetBackground(1, 1, 1);
	ren2->AddActor(imgActor);

	//left, bottom(contour)
	vtkSmartPointer<vtkRenderer> ren3 = vtkSmartPointer<vtkRenderer>::New();
	ren3->SetViewport(0, 0, 0.49, 0.49);
	ren3->SetBackground(1, 1, 1);
	ren3->AddActor(contourActor);

	//right, bottom(3D)
	vtkSmartPointer<vtkRenderer> ren4 = vtkSmartPointer<vtkRenderer>::New();
	ren4->SetViewport(0.51, 0, 1, 0.49);
	ren4->SetBackground(1, 1, 1);
	ren4->AddActor(demActor);

	//7. RenderWindow.
	vtkSmartPointer<vtkRenderWindow> renWin = vtkSmartPointer<vtkRenderWindow>::New();
	renWin->SetSize(800, 600);
	renWin->AddRenderer(ren1);
	renWin->AddRenderer(ren2);
	renWin->AddRenderer(ren3);
	renWin->AddRenderer(ren4);
	//ren1->SetViewport(0, 0, 1, 1);

	ren4->GetActiveCamera()->SetViewUp(0, 0, 1);
	ren4->GetActiveCamera()->SetPosition(-99900, -21354, 131801);
	ren4->GetActiveCamera()->SetFocalPoint(41461, 41461, 2815);
	ren4->ResetCamera();
	ren4->ResetCameraClippingRange();

	vtkSmartPointer<vtkRenderWindowInteractor> iren = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	iren->SetRenderWindow(renWin);

	renWin->Render();
	iren->Initialize();
	iren->Start();

	return EXIT_SUCCESS;
}
