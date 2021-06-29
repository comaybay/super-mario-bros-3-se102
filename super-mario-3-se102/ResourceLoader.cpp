#include "ResourceLoader.h"
#include "TextureManager.h"
#include "AnimationManager.h"
#include "HitboxManager.h"
#include "ProcessingUtils.h"
#include <fstream>

using namespace ProcessingUtils;
using namespace Utils;

ResourceLoader::ResourceLoader(const std::string& rootDirectory)
	: root(rootDirectory) {}

void ResourceLoader::GetGameSettings(Dimension& gameDimension, int& pixelScale, int& maxFPS, std::string& dataDirectory) const
{
	std::string configPath = JoinPath(root, "/config.txt");
	std::ifstream file(configPath);

	std::string line = "";
	while (line != "EOF") {
		line = GetNextNonCommentLine(file);
		if (line[0] != '[' || line != "[GAME SETTINGS]")
			continue;

		line = GetNextNonCommentLine(file);
		std::vector<std::string> dimTokens = SplitByComma(line);
		if (dimTokens.size() != 2)
			throw InvalidTokenSizeException(2);

		gameDimension = Dimension(stoi(dimTokens[0]), stoi(dimTokens[1]));

		line = GetNextNonCommentLine(file);
		std::vector<std::string> scaleToken = SplitByComma(line);
		if (scaleToken.size() != 1)
			throw InvalidTokenSizeException(1);

		pixelScale = stoi(scaleToken[0]);

		line = GetNextNonCommentLine(file);
		std::vector<std::string> fpsToken = SplitByComma(line);
		if (fpsToken.size() != 1)
			throw InvalidTokenSizeException(1);

		maxFPS = stoi(fpsToken[0]);

		line = GetNextNonCommentLine(file);
		std::vector<std::string> dirToken = SplitByComma(line);
		if (dirToken.size() != 1)
			throw InvalidTokenSizeException(1);

		dataDirectory = dirToken[0];

		file.close();
		return;
	}

	file.close();
	throw std::exception("GetGameSettings Failed: missing [GAME SETTINGS] section");
}

void ResourceLoader::Load() const
{
	std::string configPath = JoinPath(root, "/config.txt");
	std::ifstream file(configPath);
	//go to section
	std::string section;
	while (std::getline(file, section)) {
		if (section[0] != '[' || section == "[GAME SETTINGS]")
			continue;

		std::string path;
		std::getline(file, path);
		path = JoinPath(root, path);

		if (section == "[TEXTURES]")
			LoadTextures(path);
		else if (section == "[ANIMATIONS]")
			LoadAnimations(path);
		else if (section == "[TILES]")
			LoadTilesTextures(path);
		else if (section == "[HITBOXES]")
			LoadHitboxes(path);
	}

	file.close();
}

void ResourceLoader::LoadTextures(const std::string& configPath) const
{
	std::ifstream file(configPath);

	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> paramTokens = SplitByComma(line);
		if (paramTokens.size() != 2)
			throw InvalidTokenSizeException(2);

		std::getline(file, line);
		std::vector<std::string> colorKeyTokens = SplitByComma(line);
		if (colorKeyTokens.size() != 3)
			throw InvalidTokenSizeException(3);

		std::string id = paramTokens[0];
		std::string path = JoinPath(root, paramTokens[1]);
		D3DCOLOR colorKey = D3DCOLOR_XRGB(stoi(colorKeyTokens[0]), stoi(colorKeyTokens[1]), stoi(colorKeyTokens[2]));

		TextureManager::Load(id, path, colorKey);
	}

	file.close();
}

void ResourceLoader::LoadAnimations(const std::string& configPath) const
{
	std::ifstream file(configPath);

	std::string line;
	while (std::getline(file, line))
	{
		if (line[0] == '#' || line == "")
			continue;

		std::vector<std::string> paramTokens = SplitByComma(line);
		if (paramTokens.size() != 4)
			throw InvalidTokenSizeException(4);

		std::string animationId = paramTokens[0];
		std::string animationType = paramTokens[1];
		float frameDuration = std::stof(paramTokens[2]);
		std::string spriteSequenceHandlingMode = paramTokens[3];

		std::getline(file, line);
		std::vector<std::string> idToken = SplitByComma(line);
		if (idToken.size() != 1)
			throw InvalidTokenSizeException(1);

		std::string textureId = idToken[0];

		std::vector<SpriteBox> spriteBoxSequence;
		if (spriteSequenceHandlingMode == "Auto") {
			std::getline(file, line);
			std::vector<std::string> rectSequenceTokens = SplitByComma(line);

			if (rectSequenceTokens.size() < 6)
				throw InvalidTokenSizeException(6);

			if (rectSequenceTokens.size() > 8)
				throw InvalidTokenSizeException(8);

			Vector2<int> startPosition(
				std::stoi(rectSequenceTokens[0]),
				std::stoi(rectSequenceTokens[1])
			);

			Dimension dim(
				std::stoi(rectSequenceTokens[2]),
				std::stoi(rectSequenceTokens[3])
			);

			int space = std::stoi(rectSequenceTokens[4]);
			int frameCount = std::stoi(rectSequenceTokens[5]);

			Vector2<int> offset(0, 0);
			if (rectSequenceTokens.size() == 8) {
				offset.x = std::stoi(rectSequenceTokens[6]);
				offset.y = std::stoi(rectSequenceTokens[7]);
			}

			spriteBoxSequence = CreateSpriteBoxSequence(startPosition, dim, space, frameCount, offset);
		}
		else if (spriteSequenceHandlingMode == "Manual") {
			while (getline(file, line)) {
				if (line == "")
					break;

				std::vector<std::string> rectSequenceTokens = SplitByComma(line);
				if (rectSequenceTokens.size() < 4)
					throw InvalidTokenSizeException(4);

				if (rectSequenceTokens.size() > 6)
					throw InvalidTokenSizeException(6);

				RECT rect{
					std::stoi(rectSequenceTokens[0]),
					std::stoi(rectSequenceTokens[1]),
					std::stoi(rectSequenceTokens[2]) + 1,
					std::stoi(rectSequenceTokens[3]) + 1,
				};

				Vector2<int> offset(0, 0);
				if (rectSequenceTokens.size() == 6) {
					offset.x = std::stoi(rectSequenceTokens[4]);
					offset.y = std::stoi(rectSequenceTokens[5]);
				}

				spriteBoxSequence.emplace_back(rect, offset);
			}

		}
		else throw std::exception("Invalid sprite box handling mode: expected Auto or Manual");

		LPDIRECT3DTEXTURE9 texture = TextureManager::Get(textureId);

		AnimationType animType;
		if (animationType == "Normal")
			animType = AnimationType::NORMAL;
		else if (animationType == "Fixed")
			animType = AnimationType::FIXED;
		else
			throw std::exception("Invalid AnimationType: expected Normal or Fixed");

		AnimationProps animProps;

		animProps = AnimationProps(animType, animationId, frameDuration, texture, spriteBoxSequence);
		AnimationManager::Add(animationId, animProps);
	}

	file.close();
}

std::vector<SpriteBox> ResourceLoader::CreateSpriteBoxSequence(
	Vector2<int> startPosition, Dimension dimension, int space, int frameCount, Vector2<int> offset) const
{
	std::vector<SpriteBox> sequence;
	for (int i = 0; i < frameCount; i++) {
		Vector2<int> position(
			startPosition.x + (dimension.width + space) * i,
			startPosition.y
		);

		sequence.emplace_back
		(
			RECT{
				(int)position.x,
				(int)position.y,
				(int)(position.x + dimension.width),
				(int)(position.y + dimension.height),
			},
			offset
			);
	}
	return sequence;
}

void ResourceLoader::LoadTilesTextures(const std::string& configPath) const
{
	std::ifstream file(configPath);

	LoadTilesTexture(file, TextureId::WORLD_TILES);
	LoadTilesTexture(file, TextureId::WORLD_MAP_TILES);

	file.close();
}
void ResourceLoader::LoadTilesTexture(std::ifstream& file, const std::string& textureId) const
{
	std::string line = GetNextNonCommentLine(file);
	std::vector<std::string> paramTokens = SplitByComma(line);
	if (paramTokens.size() != 1)
		throw InvalidTokenSizeException(1);

	line = GetNextNonCommentLine(file);
	std::vector<std::string> colorKeyTokens = SplitByComma(line);
	if (colorKeyTokens.size() != 3)
		throw InvalidTokenSizeException(3);

	std::string path = JoinPath(root, paramTokens[0]);
	D3DCOLOR colorKey = D3DCOLOR_XRGB(stoi(colorKeyTokens[0]), stoi(colorKeyTokens[1]), stoi(colorKeyTokens[2]));

	TextureManager::Load(textureId, path, colorKey);
}

void ResourceLoader::LoadHitboxes(const std::string& configPath) const
{
	std::ifstream file(configPath);

	std::string line;
	while (true) {
		line = GetNextNonCommentLine(file);
		if (line == "EOF")
			return;

		std::vector<std::string> tokens = SplitByComma(line);
		if (tokens.size() != 5)
			throw InvalidTokenSizeException(5);

		LPHitbox hitbox = new Hitbox(
			Vector2<float>(stof(tokens[1]), stof(tokens[2])),
			Dimension(stoi(tokens[3]), stoi(tokens[4]))
		);

		HitboxManager::Add(tokens[0], hitbox);
	}
}


