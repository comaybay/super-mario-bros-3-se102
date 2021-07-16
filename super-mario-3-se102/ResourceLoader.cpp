#include "Game.h"
#include "ResourceLoader.h"
#include "TextureManager.h"
#include "AnimationManager.h"
#include "HitboxManager.h"
#include "ProcessingUtils.h"
#include <fstream>

using namespace ProcessingUtils;
using namespace Utils;

ResourceLoader::ResourceLoader(const std::string& dataDirectory) :
	dataDirectory(dataDirectory)
{
}

void ResourceLoader::Load()
{
	std::string configPath = JoinPath(dataDirectory, "/config.txt");
	std::ifstream file(configPath);

	std::string section = "";
	while (section != "EOF") {
		section = GetNextNonCommentLine(file);

		//skip game settings section and unrelated texts
		if (section[0] != '[' || section == "[GAME SETTINGS]")
			continue;

		std::string path = JoinPath(dataDirectory, GetNextNonCommentLine(file));
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

void ResourceLoader::LoadTextures(const std::string& configPath)
{
	std::ifstream file(configPath);

	while (true)
	{
		std::string line = GetNextNonCommentLine(file);
		if (line == "EOF")
			break;

		std::vector<std::string> paramTokens = SplitByComma(line);
		if (paramTokens.size() != 2)
			throw InvalidTokenSizeException(2);

		std::vector<std::string> colorKeyTokens = SplitByComma(GetNextNonCommentLine(file));
		if (colorKeyTokens.size() != 3)
			throw InvalidTokenSizeException(3);

		std::string& id = paramTokens[0];
		std::string path = JoinPath(dataDirectory, paramTokens[1]);
		D3DCOLOR colorKey = D3DCOLOR_XRGB(stoi(colorKeyTokens[0]), stoi(colorKeyTokens[1]), stoi(colorKeyTokens[2]));

		TextureManager::Load(id, path, colorKey);
	}

	file.close();
}

void ResourceLoader::LoadAnimations(const std::string& configPath)
{
	std::ifstream file(configPath);

	while (true)
	{
		std::string line = GetNextNonCommentLine(file);
		if (line == "EOF")
			break;

		std::vector<std::string> paramTokens = SplitByComma(line);
		if (paramTokens.size() != 4)
			throw InvalidTokenSizeException(4);

		std::string& animationId = paramTokens[0];
		std::string& animationType = paramTokens[1];
		float frameDuration = std::stof(paramTokens[2]);
		std::string& spriteSequenceHandlingMode = paramTokens[3];

		std::vector<std::string> idToken = SplitByComma(GetNextNonCommentLine(file));
		if (idToken.size() != 1)
			throw InvalidTokenSizeException(1);

		std::string& textureId = idToken[0];

		std::vector<SpriteBox> spriteBoxSequence;
		if (spriteSequenceHandlingMode == "Auto")
			spriteBoxSequence = CreateSpriteBoxSequenceAuto(file);

		else if (spriteSequenceHandlingMode == "Manual")
			spriteBoxSequence = CreateSpriteBoxSequenceManual(file);

		else throw std::exception("Invalid sprite box handling mode: expected Auto or Manual");

		LPDIRECT3DTEXTURE9 texture = TextureManager::Get(textureId);

		AnimationType animType;
		if (animationType == "Normal")
			animType = AnimationType::NORMAL;

		else if (animationType == "Fixed")
			animType = AnimationType::FIXED;

		else
			throw std::exception("Invalid AnimationType: expected Normal or Fixed");

		AnimationManager::Add(animationId, { animType, animationId, frameDuration, texture, spriteBoxSequence });
	}

	file.close();
}

std::vector<SpriteBox> ResourceLoader::CreateSpriteBoxSequenceManual(std::ifstream& file)
{
	std::string line;
	std::vector<SpriteBox> sequence;

	//use getline instead of GetNextNonCommentLine to detect newline
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

		sequence.emplace_back(rect, offset);
	}
	return sequence;
}

std::vector<SpriteBox> ResourceLoader::CreateSpriteBoxSequenceAuto(std::ifstream& file)
{
	std::vector<std::string> rectSequenceTokens = SplitByComma(GetNextNonCommentLine(file));

	if (rectSequenceTokens.size() < 6)
		throw InvalidTokenSizeException(6);

	if (rectSequenceTokens.size() > 8)
		throw InvalidTokenSizeException(8);

	Vector2<int> startPosition(
		std::stoi(rectSequenceTokens[0]),
		std::stoi(rectSequenceTokens[1])
	);

	Dimension<int> dim(
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

	return CreateSpriteBoxSequence(startPosition, dim, space, frameCount, offset);
}

std::vector<SpriteBox> ResourceLoader::CreateSpriteBoxSequence(
	const Vector2<int>& startPosition, const Dimension<int>& dimension, int space, int frameCount, const Vector2<int>& offset)
{
	std::vector<SpriteBox> sequence;
	for (int i = 0; i < frameCount; i++) {
		Vector2<int> position(
			startPosition.x + (int)(dimension.width + space) * i,
			startPosition.y
		);

		sequence.emplace_back(
			RECT{
				position.x,
				position.y,
				position.x + dimension.width,
				position.y + dimension.height,
			},
			offset
			);
	}
	return sequence;
}

void ResourceLoader::LoadTilesTextures(const std::string& configPath)
{
	std::ifstream file(configPath);

	LoadTilesTexture(file, TextureId::WORLD_TILES);
	LoadTilesTexture(file, TextureId::WORLD_MAP_TILES);

	file.close();
}

void ResourceLoader::LoadTilesTexture(std::ifstream& file, const std::string& textureId)
{
	std::vector<std::string> paramTokens = SplitByComma(GetNextNonCommentLine(file));
	if (paramTokens.size() != 1)
		throw InvalidTokenSizeException(1);

	std::vector<std::string> colorKeyTokens = SplitByComma(GetNextNonCommentLine(file));
	if (colorKeyTokens.size() != 3)
		throw InvalidTokenSizeException(3);

	std::string path = JoinPath(dataDirectory, paramTokens[0]);
	D3DCOLOR colorKey = D3DCOLOR_XRGB(stoi(colorKeyTokens[0]), stoi(colorKeyTokens[1]), stoi(colorKeyTokens[2]));

	TextureManager::Load(textureId, path, colorKey);
}

void ResourceLoader::LoadHitboxes(const std::string& configPath)
{
	std::ifstream file(configPath);

	std::string line;
	while (true) {
		line = GetNextNonCommentLine(file);
		if (line == "EOF")
			break;

		std::vector<std::string> tokens = SplitByComma(line);
		if (tokens.size() != 5)
			throw InvalidTokenSizeException(5);

		LPHitbox hitbox = new Hitbox(
			Vector2<float>(stof(tokens[1]), stof(tokens[2])),
			Dimension<int>(stoi(tokens[3]), stoi(tokens[4]))
		);

		HitboxManager::Add(tokens[0], hitbox);
	}

	file.close();
}


