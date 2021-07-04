#pragma once
#include "Utils.h"

struct GameSettings {
	Utils::Dimension<float> gameDimension;
	int pixelScale;
	int maxFPS;
	bool pixelPerfectRendering;
	std::string dataDirectory;
	GameSettings(const Utils::Dimension<float>& gameDimension, int pixelScale, int maxFPS, bool pixelPerfectRendering, const std::string& dataDirectory);
	GameSettings();
};