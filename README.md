Generate Contours with Visualization Toolkit
=========

The sample shows how to use [Visualization Toolkit][1] to generate contours.

Screenshots
-----------
![image](http://www.codepool.biz/wp-content/uploads/2014/10/vtk_viewport.png)

Resources
-----------

* [VTK-6.1.0][2]
* [CMake][3]

How to Run:
-----------
1. Build the VTK source code to generate dependency libraries.
2. Specify the directories of VTK head files and libraries.
![image](http://www.codepool.biz/wp-content/uploads/2014/10/vtk_directories.png)
3. Add the following dependency libs:
![image](http://www.codepool.biz/wp-content/uploads/2014/10/vtk_libs.png)

  ```sh
vtkRenderingOpenGL-6.1.lib;
vtkRenderingCore-6.1.lib;
vtkInteractionStyle-6.1.lib;
vtkCommonSystem-6.1.lib;
vtkCommonCore-6.1.lib;
vtkCommonTransforms-6.1.lib;
vtkCommonMisc-6.1.lib;
vtkCommonExecutionModel-6.1.lib;
vtkCommonDataModel-6.1.lib;
vtkCommonMath-6.1.lib;
vtkFiltersCore-6.1.lib;
vtkFiltersSources-6.1.lib;
vtkFiltersGeneral-6.1.lib;
vtkFiltersGeometry-6.1.lib;
vtkFiltersExtraction-6.1.lib;
vtkImagingCore-6.1.lib;
vtkIOImage-6.1.lib;
vtksys-6.1.lib;
  ```
4. Build the project, and run the demo.


[1]:http://www.vtk.org/
[2]:http://www.vtk.org/VTK/resources/software.html
[3]:http://www.cmake.org/download/
