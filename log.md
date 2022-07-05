# Project log book
## Meeting Jun. 10, 2022
### Plan for coming week
- Make final decision on project topic: Light Field Display, or Gesture Recognation, or SLAM

## Meeting Jun. 17, 2022
### Plan for previous week
- Make final decision on project topic: Light Field Display, or Gesture Recognation, or SLAM
### Progress during previous week
- Completed: 
- - Researched on principle and industrial applications of the Light Field Display
### Plan for coming week
- Plot point cloud image on the display

## Meeting Jun. 23, 2022
### Plan for previous week
- Plot point cloud image on the display
### Progress during previous week
#### Jun. 17, 2022 log
- Completed: 
- - Study the .ply file format (one of common formats for point cloud image)
- Made progress:
- - Set up environment: install IDE & SDK; 85% complete
- - Code to read .ply file: subroutine `readPLY`; 30% complete
- Encountered problem:
- - Don't know the supported data format of the looking glass. Need to request for SDK (waiting for manufacturer's response)

#### Jun. 19, 2022
- Completed:
- - PLY file decoder is able to read simple PLY file (based on samples I have)
- Made progress:
- - Studying HoloPlay SDK; 10% complete

#### Jun. 20, 2022
- Made progress:
- - Studying HoloPlay SDK; 50% complete. Found the entry function to generate 3D model dataset and start processing. May import decoded PLY data from `readPLY` to this function to test. 
- Encountered problem:
- - Found out my data should be pre-rendered before using the SDK (Stated in official documentation). 3D rendering may exceed my coding ability. Looking for open source library. 

#### Jun. 22, 2022
- Completed:
- - Successfully built VTK (an open source C++ library to render 3D Image, and integrated with HoloPlay SDK to output to the display)
- - Wrote instruction to build VTK (since there are 4 cross-referencing official documentations (they have errors as well))
- Made progress:
- - Studying VTK; 10% complete
- Encountered problem:
- - 3D images has to be rendered before passing to SDK. Therefore, stopped studying SDK. Switch to VTK. 
- - Found out VTK has an example to decode PLY file. Switch to VTK. 

#### Jun. 23, 2022
- Completed:
- - Changed original code to C++ for easier integrating with VTK
- - The project is able to plot point cloud image to the display

### Plan for coming week
- Create GUI using C++ to control viewing angle of the point cloud image
- Show 3D animation on the display

## Meeting Jul. 1, 2022
### Plan for previous week
- Create GUI using C++ to control viewing angle of the point cloud image
- Show 3D animation on the display
### Progress during previous week
#### Jun. 24, 2022
- Made progress:
- - Study QT (a framework based on C++, to create GUI for applications with cross-platform ability)
- - Study wxWidget (a C++ library to create GUI for applicaitons)

#### Jun. 27, 2022
- Made progress:
- - Study QT. Try to add gamepad controller support as it would be more fun to younger students to learn this project

#### Jun. 29, 2022
- Made progress:
- - Study QT. 
- - Try to make a loop in `vtkHandler` for user input to change view direction of the 3D model. 
- Encountered problem:
- - Render window becomes unresponsive after a few loops. Worried about animation possibility. Need to study the rendering process.

#### Jun. 30, 2022
- Made progress:
- - Study VTK APIs. 

### Plan for coming week
- Debugging Code (Fix horizontal angle controlling)

## Meeting Jul. 8, 2022
### Plan for previous week
- Debugging Code (Fix horizontal angle controlling)

### Progress during previous week
#### Jul. 1, 2022
- Completed:
- - Fixed screen frozen problem by calling `renderWindow->Finalize()`
- Encountered problem:
- - Rendering process takes long time; Seek solution from VTK's guide.

#### Jul. 4, 2022
- Completed:
- - Found out the path from 3D camera to display: Intel RealSense Camera -> camera's SDK `rs-pointcloud` -> Point Cloud Library -> VTK -> Looking Glass SDK -> Looking Glass Display
- Made progress:
- - Building Environment for Point Cloud Library; 20%. Installing prereq; 30%
- Encountered problem:
- - Point Cloud Library has a mandatary prereq `FLANN` which no longer been maintainanced. Looking for other ways to build. 

#### Jul. 5, 2022

### Plan for coming week