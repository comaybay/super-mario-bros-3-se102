#pragma once
#include <string>
#include "GameSettings.h"
#include "Animation.h"

class ResourceLoader
{
public:
	ResourceLoader(const std::string& dataDirectory);
	void Load();
private:
	void LoadTextures(const std::string& configPath);
	void LoadAnimations(const std::string& configPath);
	void LoadTilesTextures(const std::string& configPath);
	void LoadTilesTexture(std::ifstream& file, const std::string& textureId);
	void LoadHitboxes(const std::string& configPath);
	std::vector<SpriteBox> CreateSpriteBoxSequenceManual(std::ifstream& file);
	std::vector<SpriteBox> CreateSpriteBoxSequenceAuto(std::ifstream& file);
	std::vector<SpriteBox> CreateSpriteBoxSequence(
		const Utils::Vector2<int>& startPosition, const Utils::Dimension<int>& dimension, int space, int frameCount,
		const Utils::Vector2<int>& offset);

	std::string dataDirectory;

};

