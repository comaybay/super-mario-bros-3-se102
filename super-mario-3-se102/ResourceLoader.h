#pragma once
#include <string>
#include "GameSettings.h"
#include "Animation.h"

class ResourceLoader
{
public:
	ResourceLoader(const std::string& rootDirectory);
	GameSettings GetGameSettings() const;
	void Load() const;
private:
	void LoadTextures(const std::string& configPath) const;
	void LoadAnimations(const std::string& configPath) const;
	void LoadTilesTextures(const std::string& configPath) const;
	void LoadTilesTexture(std::ifstream& file, const std::string& textureId) const;
	void LoadHitboxes(const std::string& configPath) const;
	std::vector<SpriteBox> CreateSpriteBoxSequence(
		Utils::Vector2<int> startPosition, Utils::Dimension<float> dimension, int space, int frameCount, Utils::Vector2<int> offset) const;

	std::string root;
};

