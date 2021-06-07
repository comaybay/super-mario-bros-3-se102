#include "ResourceLoader.h"
#include "Utils.h"
#include "TextureManager.h"
#include "Animation.h"
#include "AnimationManager.h"
#include <fstream>
#include <vector>

ResourceLoader::ResourceLoader(std::string rootDirectory)
	: root(rootDirectory) {}

void ResourceLoader::Load() const
{
	std::string configPath = Utils::JoinPath(root, "/config.txt");
	std::ifstream file(configPath);
	//go to section
	std::string section;
	while (std::getline(file, section)) {
		if (section[0] != '[')
			continue;

		std::string path;
		std::getline(file, path);
		path = Utils::JoinPath(root, path);

		if (section == "[TEXTURES]")
			LoadTextures(path);
		else if (section == "[ANIMATIONS]")
			LoadAnimations(path);
		else if (section == "[Tiles]")
			LoadTiles(path);
	}

	file.close();
}

void ResourceLoader::LoadTextures(std::string configPath) const
{
	std::ifstream file(configPath);

	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> paramTokens = Utils::SplitByComma(line);
		if (paramTokens.size() != 2)
			throw Utils::InvalidTokenSizeException(2);

		std::getline(file, line);
		std::vector<std::string> colorKeyTokens = Utils::SplitByComma(line);
		if (colorKeyTokens.size() != 3)
			throw Utils::InvalidTokenSizeException(3);

		std::string id = paramTokens[0];
		std::string path = Utils::JoinPath(root, paramTokens[1]);
		D3DCOLOR colorKey = D3DCOLOR_XRGB(stoi(colorKeyTokens[0]), stoi(colorKeyTokens[1]), stoi(colorKeyTokens[2]));

		TextureManager::Load(id, path, colorKey);
	}

	file.close();
}

void ResourceLoader::LoadAnimations(std::string configPath) const
{
	std::ifstream file(configPath);

	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> paramTokens = Utils::SplitByComma(line);
		if (paramTokens.size() != 3)
			throw Utils::InvalidTokenSizeException(3);

		std::string animationId = paramTokens[0];
		float frameDuration = std::stof(paramTokens[1]);
		std::string rectSequenceHandlingMode = paramTokens[2];

		std::getline(file, line);
		std::vector<std::string> idToken = Utils::SplitByComma(line);
		if (idToken.size() != 1)
			throw Utils::InvalidTokenSizeException(1);

		std::string textureId = idToken[0];

		std::vector<Utils::SpriteBox> spriteBoxSequence;
		if (rectSequenceHandlingMode == "Auto") {
			std::getline(file, line);
			std::vector<std::string> rectSequenceTokens = Utils::SplitByComma(line);

			if (rectSequenceTokens.size() < 6)
				throw Utils::InvalidTokenSizeException(6);

			if (rectSequenceTokens.size() > 8)
				throw Utils::InvalidTokenSizeException(8);

			Utils::Vector2 startPosition(
				std::stoi(rectSequenceTokens[0]),
				std::stoi(rectSequenceTokens[1])
			);

			Utils::Dimension dim(
				std::stoi(rectSequenceTokens[2]),
				std::stoi(rectSequenceTokens[3])
			);

			int space = std::stoi(rectSequenceTokens[4]);
			int frameCount = std::stoi(rectSequenceTokens[5]);

			Utils::Vector2 offset(0, 0);
			if (rectSequenceTokens.size() == 8) {
				offset.x = std::stoi(rectSequenceTokens[6]);
				offset.y = std::stoi(rectSequenceTokens[7]);
			}

			spriteBoxSequence = Utils::CreateSpriteBoxSequence(startPosition, dim, space, frameCount, offset);
		}
		else if (rectSequenceHandlingMode == "Manual") {
			while (getline(file, line)) {
				if (line == "")
					break;

				std::vector<std::string> rectSequenceTokens = Utils::SplitByComma(line);
				if (rectSequenceTokens.size() < 4)
					throw Utils::InvalidTokenSizeException(4);

				if (rectSequenceTokens.size() > 6)
					throw Utils::InvalidTokenSizeException(6);

				RECT rect{
					std::stoi(rectSequenceTokens[0]),
					std::stoi(rectSequenceTokens[1]),
					std::stoi(rectSequenceTokens[2]) + 1,
					std::stoi(rectSequenceTokens[3]) + 1,
				};

				Utils::Vector2 offset(0, 0);
				if (rectSequenceTokens.size() == 6) {
					offset.x = std::stoi(rectSequenceTokens[4]);
					offset.y = std::stoi(rectSequenceTokens[5]);
				}

				spriteBoxSequence.emplace_back(rect, offset);
			}

		}
		else
			throw std::exception("Invalid sprite box handling mode: expected Auto or Manual");

		LPDIRECT3DTEXTURE9 texture = TextureManager::Get(textureId);
		LPAnimation animation = new Animation(animationId, frameDuration, texture, spriteBoxSequence);

		AnimationManager::Add(animationId, animation);
	}

	file.close();
}

void ResourceLoader::LoadTiles(std::string configPath) const
{
	std::ifstream file(configPath);

	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> paramTokens = Utils::SplitByComma(line);
		if (paramTokens.size() != 1)
			throw Utils::InvalidTokenSizeException(1);

		std::getline(file, line);
		std::vector<std::string> colorKeyTokens = Utils::SplitByComma(line);
		if (colorKeyTokens.size() != 3)
			throw Utils::InvalidTokenSizeException(3);

		std::string path = Utils::JoinPath(root, paramTokens[0]);
		D3DCOLOR colorKey = D3DCOLOR_XRGB(stoi(colorKeyTokens[0]), stoi(colorKeyTokens[1]), stoi(colorKeyTokens[2]));

		TextureManager::Load(TextureID::TILES, path, colorKey);
	}

	file.close();
}
