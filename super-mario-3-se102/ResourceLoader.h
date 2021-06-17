#pragma once
#include <string>
#include "Utils.h"

class ResourceLoader
{
public:
	ResourceLoader(const std::string& rootDirectory);
	void GetGameSettings(Utils::Dimension& gameDimension, int& pixelScale) const;
	void Load() const;
private:
	void LoadTextures(const std::string& configPath) const;
	void LoadAnimations(const std::string& configPath) const;
	void LoadTiles(const std::string& configPath) const;
	void LoadHitboxes(const std::string& configPath) const;
	std::string root;
};

