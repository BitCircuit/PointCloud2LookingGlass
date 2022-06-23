# Project log book
## Meeting Jun. 10, 2022
### Plan for coming week
- Make final decision on project topic: Light Field Display, or Gesture Recognation, or SLAM

## Meeting Jun. 17, 2022
### Plan for previous week
- Make final decision on project topic: Light Field Display, or Gesture Recognation, or SLAM
### Progress during previous week
- Completed: 
- - Researched on principle, applications and marketing value of the Light Field Display
### Plan for coming week
- Plot point cloud image on the display

## Meeting Jun. 24, 2022
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
- - Studying HoloPlay SDK; 50% complete. Found the data port. May import decoded PLY data from `readPLY` to this function to test. 
- Encountered problem:
- - Requirement from official documents states my data should be pre-rendered before using the SDK. 3D rendering may exceed my coding ability. Looking for open source library. 

#### Jun. 23, 2022
- Completed:
- - Successfully built VTK (an open source C++ library to render 3D Image, and integrated with HoloPlay SDK to output to the display)
- - Wrote instruction to build VTK (since there are 4 cross-referencing official documentations (they have errors as well))
- Made progress:
- - Studying VTK; 10% complete
- Encountered problem:
- - 3D images has to be rendered before passing to SDK. Therefore, stopped studying SDK. Switch to VTK. 
- - Found out VTK has an example to decode PLY file. Switch to VTK. 

### Plan for coming week