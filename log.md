# Project log book

## Jun. 17, 2022
- Search online about point cloud file format. There are many common formats: .xyz, .ply, .pcd, etc. 
- Google search 3D models to download. Sketchfab.com provides some free models. Only ply formatted files are available to download. 
- Start studying PLY file format. 
> Normal text. Each line ending with LF (0x0A). The first line contains `ply` indicates the file format.  
> The second line indicates the type (ascii or binary) and version.  
> Ignore lines (comment, info, etc.) before `element`  
>
> `element vertex 6782430` means there are 6782430 vertexes in the file. Each vertex may have:
> 1. `property float x` means coordinate x has float data type (4 bytes)
> 2. `property float y` means the next 4 bytes indicates coordinate y
> 3. `property float z` means the next 4 bytes indicates coordinate z
> 4. `property uchar red` means the next char (1 byte) indicate color Red
> 5. `property uchar green` means the next char (1 byte) indicate color Green
> 6. `property uchar blue` means the next char (1 byte) indicate color Blue  
>
> and other properties...  
> The next 15 bytes (as example shown above) would indicate the next element (vertex).    
> The last line `end_header<LF>` indicates the end of the header. 
- Use `Matlab` command, `pcread` from `Computer Vision Toolbox`, load the PLY file `foot_ankle_right_02.ply` into dataset. 
- Use `HxD`, an open-source software to read files in hex. Read hex values after `end_header<LF>` and compare float32 value (little endian) to the one in Matlab. Matched. And continue reading serveral more float32 values. 
- Conclusion: The data after `end_header<LF>` are exactly match the format indicate in header. And there is no any sepcial charactors (such as `<LF>`) in between (for binary formatted PLY files). In theory, ASCII formatted PLY files should be different, but there is no file available to test. 

- From https://look.glass/, `HoloPlay Services` is required to install on host computer, to be able to communicate with the looking glass devices. 
- To further develop programs, `HoloPlay SDK` is required. Request is sent to access SDK resources.  
- Start writing subroutine `readPLY` using language C (suitable for cross-platform support, especially on the MCU if needed. Not like Java/Python code runs on virtual runtime environment, C/C++ generates real machine code. )
- Use `malloc` to dynamically define arrays. 
> Header:
>> Use `fgets` function from `stdio.h` to read line by line.  
>> Use `strtok` function from `string.h` to break line by spaces.  
>> Since `end_header` has length of 10 chars. Therefore, use 12 chars (add two more chars for extra little space to prevent leak) to define `header1` array.  
>> Same for `header2` array, since `binary_little_endian` has length of 20 chars. Add two more chars for extra spaces. So `header2` has length of 22 chars.  
>> Assume the length of `header3`~`header5` would not exceed 20 chars. So they have length of 20 chars each.  
>
> Body:



- If the hex binary were recorded in little endian, convert to big endian first. 
- Since the file uses binary to record float numbers, some code were copied from https://www.geeksforgeeks.org/program-for-conversion-of-32-bits-single-precision-ieee-754-floating-point-representation/ to perform IEEE-754 floating point algorithm. 
- Since the online code is using integer array to calculate mantissa, exponent and sign parts of IEEE-754, bitwise operation is used to convert the hex value to integer array. 

## Jun. 19, 2022
- (Writing body part)---------------------
- Used `printf` to output first 50 columns of data, and compared with dataset in `Matlab`.
- The program stopped suddenly. 
- Searched online. Used `feof` and `ferror` to check for problems. 
Found out when program reads `0x1A` (same as ctrl+z in ASCII) would trigger EOF (End of File) signal. Changed `fopen` mode from `r` to `rb+` (as `Reading Binary`).

- Completed:
- - PLY file decoder is able to read simple PLY file (based on samples I have)
- Made progress:
- - Studying HoloPlay SDK; 10% complete

## Jun. 20, 2022
- Made progress:
- - Studying HoloPlay SDK; 50% complete. Found the entry function to generate 3D model dataset and start processing. May import decoded PLY data from `readPLY` to this function to test. 
- Encountered problem:
- - Found out my data should be pre-rendered before using the SDK (Stated in official documentation). 3D rendering may exceed my coding ability. Looking for open source library. 

## Jun. 22, 2022
- Completed:
- - Successfully built VTK (an open source C++ library to render 3D Image, and integrated with HoloPlay SDK to output to the display)
- - Wrote instruction to build VTK (since there are 4 cross-referencing official documentations (they have errors as well))
- Made progress:
- - Studying VTK; 10% complete
- Encountered problem:
- - 3D images has to be rendered before passing to SDK. Therefore, stopped studying SDK. Switch to VTK. 
- - Found out VTK has an example to decode PLY file. Switch to VTK. 

## Jun. 23, 2022
- Completed:
- - Changed original code to C++ for easier integrating with VTK
- - The project is able to plot point cloud image to the display

## Jun. 24, 2022
- Made progress:
- - Study QT (a framework based on C++, to create GUI for applications with cross-platform ability)
- - Study wxWidget (a C++ library to create GUI for applicaitons)

## Jun. 27, 2022
- Made progress:
- - Study QT. Try to add gamepad controller support as it would be more fun to younger students to learn this project

## Jun. 29, 2022
- Made progress:
- - Study QT. 
- - Try to make a loop in `vtkHandler` for user input to change view direction of the 3D model. 
- Encountered problem:
- - Render window becomes unresponsive after a few loops. Worried about animation possibility. Need to study the rendering process.

## Jun. 30, 2022
- Made progress:
- - Study VTK APIs. 

## Jul. 1, 2022
- After meeting, professors suggest the problem I have encountered with is the memory leakage. Tried public APIs in `renderWindow` module. 




- Completed:
- - Fixed screen frozen problem by calling `renderWindow->Finalize()`
- Encountered problem:
- - Rendering process takes long time; Seek solution from VTK's guide.

## Jul. 4, 2022
- Completed:
- - Found out the path from 3D camera to display: Intel RealSense Camera -> camera's SDK `rs-pointcloud` -> Point Cloud Library -> VTK -> Looking Glass SDK -> Looking Glass Display
- Made progress:
- - Building Environment for Point Cloud Library; 20%. Installing prereq; 30%
- Encountered problem:
- - Point Cloud Library has a mandatary prereq `FLANN` which no longer been maintainanced. Looking for other ways to build. 

## Jul. 5, 2022
- Started considering about what's next, as the camera model for this project has not been determined. Looking glass website shows some cool project ideas. One of them can gather RGB signal from iPhone's front camera and depth signal from iPhone's Face ID (for iPhone X or later) and LiDAR (from iPhone 12 or later). It requires an app called `Record3D` being installed on iPhone. After recording a RGBD video, exported it to a sequence of point cloud PLY files. `VTK` could not properly read them. It shows blank window. 
- A friend told me `3D Viewer` provided by Microsoft can read .PLY files. Tried to use `3D Viewer` to load the PLY files which `VTK` could not show result (i.e. `amphi_vertices.ply`), which I thought maybe an open-source software just doesn't have enough capabilities. `3D Viewer` could not open those either. It shows loading error. After comparing files, it turns out that files do not have `element face` could not be loaded by `3D Viewer` nor plotted by `VTK`. After searching online, the PLY files without faces element means they are not meshed. 
- Found out `point cloud library (PCL)` has ability to mesh point cloud files. 
- About the slow rendering process, considering about using multi-threading to improve the speed. 

## Jul. 6, 2022
- To detect problem, debugger (provided by Visual Studio 2022) is used. Commented `renderWindow->Finalize()`. Run the code with debugger (instead of run from command line). The program runs perfectly.  
- After checking online and asking supervisor, the VS IDE Debugger has garbage collection mechanism, the runtime would optimize memory when code is running. Decided to use external tools to detect memory leakage problem. 
- `Visual Leak Detector (VLD)` seems famous, but only support Visual Studio 2015 or earlier (Currently running VS2022). Installed and included it in the project (Hope IDE version is not a problem). The VLD returned no leaks detected.

## Jul. 7, 2022
- `Intel Inspector` seems well maintained. However, it supports Visual Studio 2017 or 2019. Therefore, installed VS2017 and run the code with it. 
- With `Intel Inspector` running, although there were some memory leak detected, the problem did not appear. 
- Since the detected memory leak occured from external libraries (vtk & system). There is nothing I can do. 
- Viewed `Issues` page in `Kitware/LookingGlassVTKModule` repo, it seems like the interactor module provided by VTK can work with looking glass module. 
- Read `4.2 Using VTK Interactors` in `VTK Users Guide`. Learnt that there are multiple methods to interact with scene (mouse, keyboard, joystick). 

## Jul. 8, 2022
- Restored code from sample which is invoking interactive module. Run the commend provided in issue page. 
- Randomly press keys (except `q` or `e` as they would exit the program as mentioned in APIs). The image moved a little bit when `w` is pressed. 
- It proves my previous observation was wrong (The interactor module is not compatiable with looking glass module). Therefore, searched online about making customized keyboard callback function. 
- Followed `KeypressEvents` sample code provided by VTK (https://kitware.github.io/vtk-examples/site/Cxx/Interaction/KeypressEvents/). Modified it to my controlling style. 
- It worked. Deleted all junk code/comments. 
- Read `4.16 Animation` and `Chapter 11 Time varying data` in `VTK Users Guide`. 

## Jul. 11, 2022
- Study Microsoft Azure Kinect Sensor SDK. URL: https://microsoft.github.io/Azure-Kinect-Sensor-SDK/master/index.html
- This SDK is only supported on Windows and Ubuntu. 
- Before proceeding to study example codes, here are some thoughts:
- - Use `k4a_device_open` to create a `device_handle` instance. (i.e. `cam`)
- - Use `k4a_device_start_cameras` with some config file to start camera. 
- - Use `k4a_device_get_capture` with `cam` to create a `capture_handle` instance. (i.e. `cap`)
- - Use `k4a_capture_get_color_image` with `cap` to get `colorImg`. 
- - Use `k4a_capture_get_depth_image` with `cap` to get `depthImg`

- - Guess: `k4a_transformation_depth_image_to_point_cloud` 
could only generate point cloud without color information?

