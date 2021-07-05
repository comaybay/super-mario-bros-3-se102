#include "GameSettings.h"

GameSettings::GameSettings(const Utils::Dimension<int>& gameDimension, int pixelScale, int maxFPS, bool pixelPerfectRendering, const std::string& dataDirectory) :
	dataDirectory(dataDirectory), gameDimension(gameDimension), pixelScale(pixelScale), maxFPS(maxFPS), pixelPerfectRendering(pixelPerfectRendering)
{
}

GameSettings::GameSettings()
{
}
