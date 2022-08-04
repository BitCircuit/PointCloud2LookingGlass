#pragma once
//#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "../Util/Util.h"

#include "json.hpp"
using json = nlohmann::json;

namespace jsonHandler {

	struct profileCamera {
		double defaultCameraPosition[3];
		double defaultViewUpVector[3];
	};
	struct mediaConfig {
		string source;
		string mediaType;
		int FPS;				// available if mediaType == "video", default to 30
		bool hasColor;
		bool isSrcHasFaceEmbed;

		double clippingRange[2];
		double windowCenter[2];
		double viewAngle;
		double focalDistance;

		bool overrideEnable;
		double overrideCameraPosition[3];
		double overrideViewUp[3];
	};

	void to_json(json&, const profileCamera&);
	void from_json(const json&, profileCamera&);
	void to_json(json&, const mediaConfig&);
	void from_json(const json&, mediaConfig&);
};

#endif // !JSONHANDLER_H

/*
Path: same as executable file
profileSource.json
{
	// Online means the file is downloaded from online
	"knownSource" : ["Kinect", "RealSense", "Record3D", "Heges", "Online"]
}

profile_kinect.json
{
	"cameraPosition" : [1.0, 0.0, 0.0],
	"viewUp" : [0.0, 0.0, 1.0]
}
*/

/*
Path: same as bunch of original PLY
videoConfig.json
{
	"source" : "Kinect", 
	"FPS" : 30,
	"hasColor" : true,
	"isSrcHasFaceEmbed" : false,		// update 
	"clippingRange" : [0.1, 0.1],
	"overrideCameraPosition" : [1.0, 0.0, 0.0],		// Not included in new JSON, only added if camera changed
	"overrideViewUp" : [0.0, 0.0, 1.0]				// Not included in new JSON, only added if camera changed
}
*/

/*
For single PLY file:
Name: same as ply file
For ply downloaded from online:
{
	"source" : "Online", 
	"hasColor" : true,
	"isSrcHasFaceEmbed" : false,		// update
	"clippingRange" : [0.1, 0.1],
	"overrideCameraPosition" : [1.0, 0.0, 0.0],		// Not included in new JSON, only added if camera changed
	"overrideViewUp" : [0.0, 0.0, 1.0]				// Not included in new JSON, only added if camera changed
}

For ply get from camera:
{
	"source" : "Kinect", 
	"hasColor" : true,
	"isSrcHasFaceEmbed" : false,		// update
	"clippingRange" : [0.1, 0.1],
	"overrideCameraPosition" : [1.0, 0.0, 0.0],		// Not included in new JSON, only added if camera changed
	"overrideViewUp" : [0.0, 0.0, 1.0]				// Not included in new JSON, only added if camera changed
}
*/