#pragma once
#include <string>
#include "Utils.h"

class ResourceLoader
{
public:
	ResourceLoader(std::string rootDirectory);
	void GetGameSettings(Utils::Dimension& gameDimension, int& pixelScale) const;
	void Load() const;
private:
	void LoadTextures(std::string configPath) const;
	void LoadAnimations(std::string configPath) const;
	void LoadTiles(std::string configPath) const;
	std::string root;
};

