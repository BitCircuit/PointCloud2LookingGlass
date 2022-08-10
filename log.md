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
>Use `k4a_device_open` to create a `device_handle` instance. (i.e. `cam`)  
>Use `k4a_device_start_cameras` with some config file to start camera.  
>Use `k4a_device_get_capture` with `cam` to create a `capture_handle` instance. (i.e. `cap`)  
>Use `k4a_capture_get_color_image` with `cap` to get `colorImg`.  
>Use `k4a_capture_get_depth_image` with `cap` to get `depthImg`

- Guess: `k4a_transformation_depth_image_to_point_cloud` 
could only generate point cloud without color information?

## Jul. 12, 2022
### Azure
- Studied Azure SDK is an open source project. However, this SDK invokes code from `Depth Engine` which contains proprietary code and is a closed source. Since `Depth Engine` is supported on Windows and Ubuntu 18.04, this project is limited to such two platforms. 
### Mesh PLY (constructing faces)
- Installed `MeshLab` and constructed surface for `amphi_vertices.ply` by using provided `Ball Pivoting Algorithm`. 
- After meshing, the `amphi_vertices.ply` file can be shown in `VTK`. 
- Tested with other PLY files (i.e. `voxel factory.bake.ply`). Confirmed that `VTK` can show color if the color information for each vertex is packed in the PLY files. (However, `3D Viewer` cannot. There is just a gray object. )
### Video frames
- While waiting for Azure 3D camera being delivered, `Record3D` can be used to learn and test how to make `VTK` taking live signal and render frames.
> Both Azure and Record3D separate RGB and Depth frames.  
> TODO: Figure out how `VTK` combine two frames. And since RGB frame has higher resolution than Depth frame, I need to match RGB pixel with Depth pixel.  
- Decided to deal with pre-recorded data first (The time-varying data). As `Record3D` could export recording video to a sequence of PLY files, TODO: make a function that can scan PLY files in a directory. 
- Tried an example code provided by VTK. While compiling, Linker error: cannot find external symbols. Example code has included many new libraries. In Windows 11, navigated to VTK install directory, find lib folder, selected all .lib files, right-click and choose `copy as path`. In `NotePad`, paste, use `find and replace` to remove all quotation marks. Back to Visual Studio 2022, paste into external dependencies. 
### Write File Scanning function
- Copied code from https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c It is a for loop to invoke `filesystem` library. 
- Used `std::vector<std::String>` in C++ for dynamic array.
- Changed solution proprity -> C/C++ -> language -> change language standard to `C++20`, as `filesystem` library requires `C++17` or later. 
- `Path` data conversion: path -`u8string()`-> string -`c_str()`-> char pointer. With `C++20`, u8string() cannot convert to string. The language standard is changed to `C++17`.
- Used `strcmp` to compare last four chars to filter file type. 
- Used `.push_back()` to add item in vector. 
- `printf` cannot print vector element. Just use `std::cout`
- With function `c_str()` transferring string element in vector to char array, `printf` can print it. 

## Jul. 13, 2022
- There are not many resources or example codes available regarding to time-varying data.
- Tried test case `TestTemporalFractal` provided by VTK. It seems like the data itself has temporal data. Not applicable to this project as the temporal vector needs to be created manually. 
- After reading some and trying to follow, there are still some gaps. Feels like it may be a wrong approach to play video. Switch to animation approach. 

## Jul. 14, 2022
- Learnt namespace and class. Change global varibles `renderer` and `renderWindow` back to local varibles. Guess it could take advantages of automatic mechanism to clean their memory after program is executed. 
- Made namespace (which has interactive keyboard callback class) separate (declare at top and define at bottom)

## Jul. 15, 2022
- Successfully play animation in VTK. Used for loop to add all frames as cues in animation scene. 
- While creating pipeline, the reader must have an input file. Initialized with the first element in the file list. 
- When adding cue: set file path for reader -> update reader -> render window (rewriting `TickInternal` API)
- setting for animation scene:
> start time: 0  
> end time: numFileScanned / FPS  
> frame rate: FPS  
> sequence mode  
- setting for each animation cue:
> time mode normalized  
> start time: frameIndex / numFileScanned  
> end time: (frameIndex + 1) / numFileScanned  
- Found problems:
> 1. It is only depth information, there is no colour yet. I can see frames are changing, but cannot recognize the picture. 
> 2. The depth information contains distance to the background wall and ceiling, makes the main actor (me) too small to see in the plot (this happens in Matlab as well). Again, this makes hard to recognize the video content. Needs to filter out some information. 
> 3. The perspective of 3D model is weird. Interactor module cannot be applied in animation (so far). Considering adding a calibration procedure for each camera model (assume the video format produced by same camera would be same). 
- For problem 3 calibration procedure:
> 1. Show the first frame from the video directory. Use interactive mode to adjust camera position. Save it as a camera profile in the program path. 
> 2. Everytime loading a video. Scan for config file in video directory, if there isn't one. Ask user to choose one camera profile, then save the choice into the video directory. 
> 3. If there is a config file in video path, then load it. Apply the camera position. Play animation as normal. 
> 4. If the perspective is still weird, config file is able to overwrite the camera position which was defined in camera profile. 
> 5. Use similar method for max depth as well. To solve problem 2. 
- More problems: 
> 1. Since the animation requires rendering in a loop. The window becomes unresponsive if another window is active (i.e. the terminal window which launched it)
> 2. To see if the memory leak problem happens again, run the code with debugger. The amount of ply files is 276, divided by 30 FPS, should be 9 seconds length. But the debugger counts the program run for 30 seconds. Need to dig for reason. 

## Jul. 17, 2022
- While searching for methods to combine color and depth frames (it is hard because of resolution difference and coordinate alignment), found that `Point Cloud Library` may be capable doing it. 
- For camera:
> 1. `PCL` has a class `OpenNI` wrapper. OpenNI is another library which can connect to any OpenNI-compatiable devices. Luckly, Kinect Azure is OpenNI compatiable. Since OpenNI supports Mac OS X (although just for Intel processor), the Kinect Azure could be used in Mac as well.  
> 2. Based on OpenNI's APIs, if Kinect Azure supports hardware sync of color and depth stream, `PCL` should be able to get point cloud with XYZ, RGB, Normal, etc. in one dataset.  
> 3. Since `PCL` works with `VTK`, it has an API called convert to VTK. It takes meshed point cloud dataset and convert to `vtkPolyData`. So, it should be able to connect directly to the pipeline in VTK.  
> 4. In `VTK`, probably need to render the window as same speed as `FPS` which can get from `OpenNI`.  
- For prerecorded video (a sequence of PLY and JPG files):
> Still searching...

## Jul. 18, 2022
- Let program check whether PLY files have face element and color information. If not, direct to output a message (0 face or 0 color) and direct to mesh algorithm (0 face). 
- Modifying `plyReader.c` I wrote at the beginning of this project. (I don't want it abondent).
> 1. Change it from language C to C++
> 2. Make it recognizing color element (already exists) and face element > 0. 
> 3. Make `colorRGBEnable` and `faceElementEnable` a public variable. 
- Gethered 3D model from another iOS app `Heges`. By using `HxD`, I found out that their exported PLY file has some special format (Right before the number of vertex and face. Instead of one space, there are 5 `0x20`). Looking for APIs in both `VTK` and `PCL`. Their code is more advanced (may cover more unseen sepcial formats).
> 1. Loading ply file to `reader`
> 2. `reader->GetOutput()` gives `vtkPolyData` type
> 3. `reader->GetOutput()->GetNumberOfPolys()` returns number of face elements. If it equals to 0, the PLY file(s) will be direct to meshing algo. 
> 4. `reader->GetOutput()->GetNumberOfElements(0)` returns number of vertex elements. 
> 5. Have not found any API under reader could return color information. 
> 6. Tried to connect reader with poly data mapper. The color map returns 0. 

## Jul. 19, 2022
- For combining a static PLY and a static JPG: 
> 1. Tried to follow sample code related to vtkFilters. 
>> a. `ImageToPolyDataFilter`: The code could not be compiled because of data type is not matched (`i2pd->SetLookupTable(quant->GetLookupTable())`).  
>> b. `CombinePolyData` seems like just adding another object to the window. 
>> c. `AlignTwoPolyDatas` seems like capable of solving alignment problem. But It needs point a. Convert image to poly data first. 
>
> 2. Searching for APIs I may use from `Point Cloud Library`. 
>> a. It doesn't have jpeg reader function. 
>
> 3. Seems like it is a mission impossible if there is no camera's parameters such as calibration data.  
> 
> Temporary conclusion: pre-recorded PLY files has to have color information in vertex or face, or only depth information can be plotted. 

- Constructe face if there is no face element present in PLY file (since point cloud generally means the points represents X, Y, and Z, does not necessary contain face element):
> Reference: `Realistic 3D Face Reconstruction Based on VTK and MFC` doi: 10.1109/ICOIP.2010.271  
> 1. Modify and integrate sample code `Delaunay2D`. The face is generated and the 3D model is plotted on the window. Problem: If there are multiple objects in the file, and their depth information have significant difference, this algorithm still connects them together. 
> 2. Based on API of `vtkDelaunay2D`. Setting Alpha (max distance) should solve the problem from point 1. 
>> a. After testing, setting Alpha to 0.075 has a good result to the sample I tested. And assume it would be a general result for all. Smaller number may result in discrete points near object's edge. Larger number may result in everything connects together. 
>> b. Found another API which sets tolerance. After testing, it would affect the surface smooth. If the tolerance too large, the object has low details (i.e. facial features may not be recognized). If the tolerance too small, the object surface may become uneven. I believe that recognizing facial features has higher priority on this matter. Best result is to comment out the command and leave it as default. :joy:  
>
> 3. Make it as a seperate method, which can be called from the beginning of other methods. Note: the face construction takes long time to compute. Save the meshed file(s), set a flag in JSON, and load meshed result next time directly. 
> 4. After testing, `vtkPLYWriter` does not write color information by default. Need to dig into reader or mapper to get color map. 
>> Solution: adding `writer->SetArrayName("RGB");` to writer.  
>> Worry about what if the original file has alpha data (transparent). That would need to be `"RGBA"` (guess from `vtkPLYReader` source code). Need a method to read if there is alpha value. Write a simplified version of `plyReader` just to determine if there is color and alpha value. 

- In `Realistic 3D Face Reconstruction Based on VTK and MFC`, `vtkTexture` is mentioned, which may map color on polydata. After running the example code `ProjectedTexture`, points are painted with color, but they are mismatched. Guess this is where the camera's parameter data is used.  

## Jul. 20, 2022
- Reconstructed my code to reduce redundence and improve readability and reusability. 
> 1. Put reuseable methods to Util.cpp, and make the method more general (i.e. `isPathAFile` can be used in both `vtkHandler` and `jsonHandler`)
> 2. Put VTK pipeline creation code seperatly as they are reused. 
> 3. Since the method `showVideo` also needs to intake `singlePLYPlot` conditionally, thinking to make `singlePLYPlot` reusable. 
- Used `MindGenius` to help me build program flowchart (kind of).
- Construted config mechanism (read/write JSON file with library `RapidJSON`) to record camera position (default or overriden by last time run) and if a file has meshed. To let animation load correct camera position and play. (interactor module does not work while animation is playing, therefore we can not change camera setting in animation)
- The code is failed to be compiled after reconstructing. Shows linker error because of multi definition. 

## Jul. 21, 2022
- To prevent multi definition, namespace is used. 
- Later found out that there are multi definition even namespace is used, because I put variables in header. To solve, I put them in source, and created get method to return the local variable. 
### jsonHandler
- Combined sample code from http://rapidjson.org/md_doc_stream.html#OStreamWrapper and https://github.com/Tencent/rapidjson/blob/master/example/simplewriter/simplewriter.cpp to create `profileCamera.json`. Need to modify a bit, such as creating two writers, one for StringBuffer, and another one for ofstream. 
- There are lots of compiler deprecation warnings exists in `RapidJSON` library. With defination of `_SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING`, there is one warning that cannot be eliminated. Consider changing to another library. 
- Changed to `nlohmann/json` (https://github.com/nlohmann/json). The reader and writer process seems more stright and easy. 

## Jul. 24, 2022 (better to explain in flowchart)
- While writing code for creating/loading/updating JSON file, keep in mind:
> 1. variables need to update:
>> a. finalReaderPath: set found one. if not, if single file, check mashed. 
>> b. finalConfigFileName: if config file exists, update it to current. If not, if video, set it fixed to be `videoConfig.json` in media directory. If single file, keep it with same name as ply file. if not meshed, append `mashed` into filename. 
>> c. finalProfileName: is being updated in `isNewProfileCreated` only, where users choose or create profile. 
>> d. camera settings: update not only json config data, but also camera values which interactor uses. 
>
> 2. if video meshed, refresh list

- Met an error that vtk shows could not load ply file, but later the PLY is plotted. By printing flag 1 & 2, the error is located. `camera settings from json` has to be performed after reader is updated with a ply file. 
- Code works for single PLY file. However, for directory, there is an error. With debugger, the variable viewer shows that my `pathParser` was not able to solve if path itself is a directory (after parsing, the result has the upper directory information). Consider comparing with file format first to see if path represent as a file. If not, just copy the whole path to result.directory. 
- `vtkPLYWriter` cannot make directory automatically. Searching online for doing it manually. 
- Followed another sample code online `RotatingSphere`, created a timer callback function to update reader index, and render new frame each time it is called. Confirmed that maybe reader and render takes long time to complete (a 4 second video, 141 frames (4 MB per raw point cloud), took about 50 mins to construct face (10 MB per meshed file) and 3 mins to play). Homever, this method enables interactor mode in video mode. Considering to pre-load frames into memory and play. 

## Jul. 25, 2022
- From discussion forum https://discourse.vtk.org/t/reducing-loading-time/6606/9, the VTK supporter mentioned we can render each frame at each time point, and save the result as each actor. To play, just switch actor on and off. After viewing `vtkRenderer` APIs, `renderer->getActors()` returns `vtkActorCollection`. Study how to intake actor collection from sample code `DetermineActorType`. 
- After testing, it seems like only the last actor showed up. It takes about 2 mins rendering frames before showing result each time when I debug. Study how to multi threading in C++
- After testing multithreads (solely with numbers, not with data), the output seems out of order. Crazy idea: create multiple (i.e. 10, same as thread num) pipelines (reader, mapper, actor, renderer, and renderWindow), and store them in vector or array. Connect. Update reader. Render. Then add each actor into main renderer, in order. 
- Data race is such a pain. By following with several online tutorials, I defined shared variables for threads by using `promise` and `future`. However, the renderer->addactor does not work well with data from threads. Error shows read access restricted, pointer was null. 
- Each thread was designed to have their own pipeline. After rendering, just output actor pointer. Maybe it was wrong. Try make main pipeline sharable to threads. 
- Found this: https://vtk.org/pipermail/vtkusers/2018-April/101443.html, it says the rendering must happen in main thread, and it should be fast. Looking for what takes time in these process. 
- After using `chrono` library to print out time stamp, I found that loading files takes really long time. 

## Jul. 26, 2022
- After randomly guessing and trying lots of ways, found this: https://public.kitware.com/pipermail/vtkusers/2003-June/018736.html. It says I have to create seperate pipeline for each actor added into `actor collection`, and then using antoher loop to add actors from collection object into renderer. 
- After trying, the debugger shows a 4.7 second video sample took 2.8 GB memory after loading (141 frames with 10 MB per frame, and took 3 mins and 15 seconds to load files with 1 thread). Even the thread sleep timer has set correctly, the playing time took about 30 seconds to complete. The memory it occupied ended at 3 GB. Maybe it is not optimal to play video?
- Multithreading is still not working as `vtkActor` or `vtkActorCollection` don't take result from threads (transferring data by using `promise` and `future`). The error just shows it is a null pointer (when the program runs to `vtkCollection` file and adding the actor. 
- Except I can export frame files to `.mp4` format and play it using `HoloPlay Studio`, I need to study how to compress video maybe? or figure out to reduce the bitrate. Normal bitrate for 1080P at 30 FPS should range 3000 - 6000 Kbps. The sample I have should probably be 240 Mbps (if the equation is 10 MB/frame * 30 FPS * 8 bit/Byte).
### Camera Integration
- NOTE: Download `point cloud library` from release page of their Github. DO NOT install from vcpkg as their official webpage shows (OpenNI wrapper will not be included). 

## Jul. 27, 2022
- Intalled `point cloud library` from all-in-one installer provided on github page. 
- Tried sample code for `point cloud library`. Found out even it is `all-in-one` it doesn't include `realsense2` wrapper. 
- Considering how to pass data from pcl to vtk. As `pcl::openni` wrapper would provide point cloud in XYZRGB, it does not provide face construction. 
- Tried other sample code such as fast triangle as it could output mesh data. 
- Wrote an interface method in vtkHandler to accept data from pcl. 
- Run code with debugger. There was an error from vtk garbage collector shows read access denied. Consider maybe the VTK version were not matched. (PCL installer has VTK 9.1, but the VTK I built is 9.2)
- Tried to build vtk 9.1 (git clone needs to add `-b "v9.1.0" --single-branch` to clone version 9.1)
- While compiling vtk 9.1, vtkLookingGlassModule (the most important one in this project) has failed. 
- Tried to build PCL. Compiler said there were lots of project build failed.
- Kept trying to build both library with some random modifications, but all failed.  

## Jul. 28. 2022
- Organized `PATH` environment. CMake now can find dependencies automatically. Yesterday I had to manuallly direct libraries and directory to CMake variables each time I tried to build libraries. 
> 1. When CMake asked for library, most likely it asks for .lib static library file. Only provide .dll dynamic library file if .lib is not presented. 
> 2. When CMake askd for directory (not for include directory), most likely it asks for directory contains CMake config files of that dependency. 
- Re-cmake both library. 
- While compiling PCL, compiler reported `boost` has x86 version, but x64 version of PCL is being built. 
- After using `vcpkg` to install dependencies in 64 bit (set `VCPKG_DEFAULT_TRIPLET = x64-windows` in system variables), re-organized `PATH` environment, and rebuilt PCL in 64 bit. 
- `pcl::VTKUtil::converttoVTK()` needs parameter of `pcl::Polygonmesh` type of data. It means the points data need to be meshed in PCL before transfer to VTK. Used tutorial code `Fast triangulation of unordered point clouds` to mesh a sample model. 
- After test, PCL and VTK can work together.

## Jul. 29, 2022
- Tried 3D model captured by Intel RealSense Camera on the Light Field Display. Found out the main charactor is out of focus. 
- Modified functions (interacting with keyboard, loading camera setting from JSON, and storing camera setting to JSON) to invoke `SetDistance()` API to move the focal point. 
- Found out `SetWindowCenter()` API cannot move the model on live on the Light Field Display. (But the model is shown at the new position when it is reopened.) 

## Aug. 2, 2022
- Intel RealSense website provides wrappers for OpenNI2 library and PCL. The OpenNI2 wrapper is chosen since Kinect Azure camera also supports OpenNI2, which means OpenNI2 is a more common. (Reference: https://dev.intelrealsense.com/docs/openni-wrapper)
- Followed the github instruction to build and compile drivers of realsense camera for OpenNI2. Run a sample program to test the driver works. 
- Used benchmark code from https://pcl.readthedocs.io/projects/tutorials/en/latest/openni_grabber.html#openni-grabber to test. The code would print out the distance between center pixel to the object. The realsense is tested successfully. 
- Trying to view realsense live signal from VTK. Converted data to VTK. The render window was not shown up. 

## Aug. 3, 2022
- Read concept of Intel Realsense D435i and Microsoft Kinect Azure. 
- Intel RealSense Camera used IR projector to transmit IR light with certain pattern. Used Bio-inspired technology. The camera used two identical cameras on left and right (inpired by human eyes) to detect depth information (Stereoscopic Vision, with computer algorithm) and capture RGB images. 
- Kinect Azure Camera used ToF technology to detect depth information. 

## Aug. 4, 2022
- Used code from https://stackoverflow.com/questions/5840148/how-can-i-get-a-files-size-in-c to get file size in Bytes (so to decide to downsample file(s) when its size exceed certain size). 
- Followed sample code provided by VTK `DownsamplePointCloud` to add filter `vtkCleanPolyData` into pipeline (between PLYReader and meshAlgo). It was expected that this filter should downsample point cloud by certain parameter. However, the vtk poped error while writing to PLY file. States that there is no data to write. 
- Used `cout` command to print point number of vtkCleanPolyData. It showed 0 points. Same result with different parameters (tolrance = 0.01, 0.1, 0.5, 0.9).
- Maybe implement downsample and mesh feature from PCL?
- While following `DownsamplePointCloud` code, found out that VTK can show point cloud directly (without meshing). Just set point size from actor. However, after testing, the render windows still shows blank. 
- After trying a PLY embedded with face element (random thought), the downsampling filter works. 
- From information printed, the generated files does not have any face element. Maybe the points are reduced, and the mesh algo's parameter becomes too small. 

## Aug. 9, 2022
- Found problem: The window center is not controllable when the looking glass pipeline is connected. -> The scene cannot be moved up, down, left, right. However, other control commands still work. 
- 