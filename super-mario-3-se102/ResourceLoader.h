#pragma once
#include <string>
#include "GameSettings.h"
#include "Animation.h"

class ResourceLoader
{
public:
	static void Load(const std::string& dataDirectory);
private:
	static void LoadTextures(const std::string& configPath, const std::string& dataDirectory);
	static void LoadAnimations(const std::string& configPath);
	static void LoadTilesTextures(const std::string& configPath, const std::string& dataDirectory);
	static void LoadTilesTexture(std::ifstream& file, const std::string& dataDirectory, const std::string& textureId);
	static void LoadHitboxes(const std::string& configPath);
	static std::vector<SpriteBox> CreateSpriteBoxSequence(
		const Utils::Vector2<int>& startPosition, const Utils::Dimension<int>& dimension, int space, int frameCount,
		const Utils::Vector2<int>& offset);

};

