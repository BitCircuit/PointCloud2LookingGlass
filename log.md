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

### Study PLY file format

- Completed: 
- - Study the .ply file format (one of common formats for point cloud image)
- Made progress:
- - Set up environment: install IDE & SDK; 85% complete
- - Code to read .ply file: subroutine `readPLY`; 30% complete
- Encountered problem:
- - Don't know the supported data format of the looking glass. Need to request for SDK (waiting for manufacturer's response)

## Jun. 19, 2022
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