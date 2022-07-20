#pragma once
#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "../Util/Util.h"

void newJSON(char*);
void readJSON();

#endif // !JSONHANDLER_H



/*
Path: same as executable file
profileCamera.json
{
	// Online means the file is downloaded from online, no need to set default values
	"knownModels" : ["Kinect", "RealSense", "Record3D", "Heges", "Online"], 
	"KinectProfile" : "profileKinect.json"			// act as path
	// ... same for other models
}

profileKinect.json
{
	"cameraPosition" : [1.0, 0.0, 0.0],
	"focalPoint" : [0.0, 0.0, 0.0],
	"viewUp" : [0.0, 0.0, 1.0], 
	"clippingRange" : [0.1, 0.1]
}
*/

/*
Path: same as bunch of original PLY
videoConfig.json
{
	"cameraModel" : "Kinect", 
	"overrideCameraPosition" : [1.0, 0.0, 0.0],		// Not included in new JSON, only added if camera changed
	"overrideFocalPoint" : [0.0, 0.0, 0.0],			// Not included in new JSON, only added if camera changed
	"overrideViewUp" : [0.0, 0.0, 1.0],				// Not included in new JSON, only added if camera changed
	"overrideClippingRange" : [0.1, 0.1],			// Not included in new JSON, only added if camera changed
	"FPS" : 30,
	"hasColor" : true,
	"isSrcHasFaceEmbed" : false,		// update 
	"meshedPLYPath" : "path"			// if isSrcHasFaceEmbed == false
}
*/

/*
For unrelated PLY files:
Name: same as ply file
For ply downloaded from online:
{
	"cameraModel" : "Online", 
	"overrideCameraPosition" : [1.0, 0.0, 0.0],		// Not included in new JSON, only added if camera changed
	"overrideFocalPoint" : [0.0, 0.0, 0.0],			// Not included in new JSON, only added if camera changed
	"overrideViewUp" : [0.0, 0.0, 1.0],				// Not included in new JSON, only added if camera changed
	"overrideClippingRange" : [0.1, 0.1],			// Not included in new JSON, only added if camera changed
}

For ply get from camera:
{
	"cameraModel" : "Kinect", 
	"overrideCameraPosition" : [1.0, 0.0, 0.0],		// Not included in new JSON, only added if camera changed
	"overrideFocalPoint" : [0.0, 0.0, 0.0],			// Not included in new JSON, only added if camera changed
	"overrideViewUp" : [0.0, 0.0, 1.0],				// Not included in new JSON, only added if camera changed
	"overrideClippingRange" : [0.1, 0.1],			// Not included in new JSON, only added if camera changed
	"hasColor" : true,
	"isSrcHasFaceEmbed" : false,		// update
	"meshedPLYPath" : "path"			// if isSrcHasFaceEmbed == false
}
*/