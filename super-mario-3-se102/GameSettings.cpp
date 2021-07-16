#include "GameSettings.h"

GameSettings::GameSettings(const Utils::Dimension<int>& gameDimension, int pixelScale, int maxFPS, bool pixelPerfectRendering,
	const std::string& dataDirectory, const std::string& initialScenePath) :
	dataDirectory(dataDirectory), gameDimension(gameDimension), pixelScale(pixelScale), maxFPS(maxFPS),
	pixelPerfectRendering(pixelPerfectRendering), initialScenePath(initialScenePath)
{
}

GameSettings::GameSettings() :
	dataDirectory(""), gameDimension({ 0, 0 }), pixelScale(0), maxFPS(0),
	pixelPerfectRendering(false), initialScenePath("")
{
}