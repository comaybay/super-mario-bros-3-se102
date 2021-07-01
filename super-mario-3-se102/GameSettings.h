#pragma once
#include "Utils.h"

struct GameSettings {
	Utils::Dimension gameDimension;
	int pixelScale;
	int maxFPS;
	std::string dataDirectory;
	GameSettings(const Utils::Dimension& gameDimension, int pixelScale, int maxFPS, const std::string& dataDirectory);
	GameSettings();
};