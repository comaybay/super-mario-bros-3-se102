#include "GameSettings.h"

GameSettings::GameSettings(const Utils::Dimension& gameDimension, int pixelScale, int maxFPS, const std::string& dataDirectory) :
	dataDirectory(dataDirectory), gameDimension(gameDimension), pixelScale(pixelScale), maxFPS(maxFPS)
{
}

GameSettings::GameSettings()
{
}
