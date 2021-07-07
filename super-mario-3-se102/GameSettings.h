#pragma once
#include "Utils.h"

struct GameSettings {
	Utils::Dimension<int> gameDimension;
	int pixelScale;
	int maxFPS;
	bool pixelPerfectRendering;
	std::string dataDirectory;
	std::string initialScenePath;
	GameSettings(const Utils::Dimension<int>& gameDimension, int pixelScale, int maxFPS,
		bool pixelPerfectRendering, const std::string& dataDirectory, const std::string& initalScenePath);
	GameSettings();
};