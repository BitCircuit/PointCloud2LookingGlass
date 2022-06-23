# About the project
## Point Cloud Image Common File Format
- PLY file. Please refer to this [README.md](/readPointCloudFile/README.md)
- PCD file. 

## External Components
### HoloPlay Service
To allow computer communicate with the Light Field Display, HoloPlay Service is a required tool to install. 

### HoloPlay SDK
To make this project compatible with the Looking Glass, SDK is a required library. However, the window generation, 3D renderer, etc. are not included in this SDK. Therefore, we need VTK. 

### VTK
VTK is a C++ library to render 3D. It has a remote module to render to the Looking Glass. To include this module, the VTK has to be built from repo. For more information, please refer to [VTKBuildingInstruction.md](/VTKBuildingInstruction.md).

HoloPlay SDK is required to build this VTK. 