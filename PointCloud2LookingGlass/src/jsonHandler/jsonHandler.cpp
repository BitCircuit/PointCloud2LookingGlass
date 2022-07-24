#include "jsonHandler.h"

void jsonHandler::to_json(json& j, const jsonHandler::profileCamera& pc) {
	j = json{ {"defaultCameraPosition", pc.defaultCameraPosition},
			{"defaultViewUpVector", pc.defaultViewUpVector} };
}

void jsonHandler::from_json(const json& j, jsonHandler::profileCamera& pc) {
	j.at("defaultCameraPosition").get_to(pc.defaultCameraPosition);
	j.at("defaultViewUpVector").get_to(pc.defaultViewUpVector);
}

void jsonHandler::to_json(json& j, const jsonHandler::mediaConfig& mc) {
	j = json{ {"source", mc.source},
			{"mediaType", mc.mediaType},
			{"FPS", mc.FPS},
			{"hasColor", mc.hasColor},
			{"isSrcHasFaceEmbed", mc.isSrcHasFaceEmbed},
			{"clippingRange", mc.clippingRange},
			{"windowCenter", mc.windowCenter},
			{"viewAngle", mc.viewAngle},
			{"overrideEnable", mc.overrideEnable},
			{"overrideCameraPosition", mc.overrideCameraPosition},
			{"overrideViewUp", mc.overrideViewUp} };
}

void jsonHandler::from_json(const json& j, jsonHandler::mediaConfig& mc) {
	j.at("source").get_to(mc.source);
	j.at("mediaType").get_to(mc.mediaType);
	j.at("FPS").get_to(mc.FPS);
	j.at("hasColor").get_to(mc.hasColor);
	j.at("isSrcHasFaceEmbed").get_to(mc.isSrcHasFaceEmbed);
	j.at("clippingRange").get_to(mc.clippingRange);
	j.at("windowCenter").get_to(mc.windowCenter);
	j.at("viewAngle").get_to(mc.viewAngle);
	j.at("overrideEnable").get_to(mc.overrideEnable);
	j.at("overrideCameraPosition").get_to(mc.overrideCameraPosition);
	j.at("overrideViewUp").get_to(mc.overrideViewUp);
}