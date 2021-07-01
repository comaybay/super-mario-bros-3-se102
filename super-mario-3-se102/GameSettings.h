#pragma once
#include "Utils.h"

struct GameSettings {
	Utils::Dimension gameDimension;
	int pixelScale;
	int maxFPS;
	bool pixelPerfectRendering;
	std::string dataDirectory;
	GameSettings(const Utils::Dimension& gameDimension, int pixelScale, int maxFPS, bool pixelPerfectRendering, const std::string& dataDirectory);
	GameSettings();
};