#include "ResourceLoader.h"
#include "TextureManager.h"
#include "FixedAnimation.h"
#include "AnimationManager.h"
#include <fstream>
#include <vector>
#include "Hitbox.h"

ResourceLoader::ResourceLoader(std::string rootDirectory)
	: root(rootDirectory) {}

void ResourceLoader::GetGameSettings(Utils::Dimension& gameDimension, int& pixelScale) const
{
	std::string configPath = Utils::JoinPath(root, "/config.txt");
	std::ifstream file(configPath);

	std::string line = "";
	while (line != "EOF") {
		line = Utils::GetNextNonCommentLine(file);
		if (line[0] != '[' || line != "[GAME SETTINGS]")
			continue;

		line = Utils::GetNextNonCommentLine(file);
		std::vector<std::string> dimTokens = Utils::SplitByComma(line);
		if (dimTokens.size() != 2)
			throw Utils::InvalidTokenSizeException(2);

		gameDimension = Utils::Dimension(stoi(dimTokens[0]), stoi(dimTokens[1]));

		line = Utils::GetNextNonCommentLine(file);
		std::vector<std::string> scaleTokens = Utils::SplitByComma(line);
		if (scaleTokens.size() != 1)
			throw Utils::InvalidTokenSizeException(1);

		pixelScale = stoi(scaleTokens[0]);

		file.close();
		return;
	}

	file.close();
	throw std::exception("GetGameSettings Failed: missing [GAME SETTINGS] section");
}

void ResourceLoader::Load() const
{
	std::string configPath = Utils::JoinPath(root, "/config.txt");
	std::ifstream file(configPath);
	//go to section
	std::string section;
	while (std::getline(file, section)) {
		if (section[0] != '[' || section == "[GAME SETTINGS]")
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
		if (paramTokens.size() != 5)
			throw Utils::InvalidTokenSizeException(5);

		std::string animationId = paramTokens[0];
		std::string animationType = paramTokens[1];
		float frameDuration = std::stof(paramTokens[2]);
		std::string hitboxHandlingMode = paramTokens[3];
		std::string spriteSequenceHandlingMode = paramTokens[4];

		std::getline(file, line);
		std::vector<std::string> idToken = Utils::SplitByComma(line);
		if (idToken.size() != 1)
			throw Utils::InvalidTokenSizeException(1);

		std::string textureId = idToken[0];

		std::vector<std::string> hitboxTokens;
		if (!Utils::VectorHas(hitboxHandlingMode, { "Custom", "Default" }))
			throw std::exception("Invalid hitbox handling mode: expected Custom or Default");

		else if (hitboxHandlingMode == "Custom") {
			line = Utils::GetNextNonCommentLine(file);
			hitboxTokens = Utils::SplitByComma(line);
			if (hitboxTokens.size() != 4)
				throw Utils::InvalidTokenSizeException(4);
		}

		std::vector<Utils::SpriteBox> spriteBoxSequence;
		if (spriteSequenceHandlingMode == "Auto") {
			std::getline(file, line);
			std::vector<std::string> rectSequenceTokens = Utils::SplitByComma(line);

			if (rectSequenceTokens.size() < 6)
				throw Utils::InvalidTokenSizeException(6);

			if (rectSequenceTokens.size() > 8)
				throw Utils::InvalidTokenSizeException(8);

			Utils::Vector2<int> startPosition(
				std::stoi(rectSequenceTokens[0]),
				std::stoi(rectSequenceTokens[1])
			);

			Utils::Dimension dim(
				std::stoi(rectSequenceTokens[2]),
				std::stoi(rectSequenceTokens[3])
			);

			int space = std::stoi(rectSequenceTokens[4]);
			int frameCount = std::stoi(rectSequenceTokens[5]);

			Utils::Vector2<int> offset(0, 0);
			if (rectSequenceTokens.size() == 8) {
				offset.x = std::stoi(rectSequenceTokens[6]);
				offset.y = std::stoi(rectSequenceTokens[7]);
			}

			spriteBoxSequence = Utils::CreateSpriteBoxSequence(startPosition, dim, space, frameCount, offset);
		}
		else if (spriteSequenceHandlingMode == "Manual") {
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

				Utils::Vector2<int> offset(0, 0);
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

		Hitbox hitbox;
		if (hitboxHandlingMode == "Default") {
			RECT hitboxRect = spriteBoxSequence[0].rect;
			Utils::Dimension dim(hitboxRect.right - hitboxRect.left, hitboxRect.bottom - hitboxRect.top);
			hitbox = Hitbox(Utils::Vector2<float>(0, 0), dim);

			animProps = AnimationProps(animType, animationId, frameDuration, texture, spriteBoxSequence, hitbox);
		}
		else if (hitboxHandlingMode == "Custom") {
			hitbox = Hitbox(
				Utils::Vector2<float>(stoi(hitboxTokens[0]), stoi(hitboxTokens[1])),
				Utils::Dimension(stoi(hitboxTokens[2]), stoi(hitboxTokens[3]))
			);

		}
		else {
			std::string msg = "Hitbox handling mode not implemented: " + hitboxHandlingMode;
			throw std::exception(msg.c_str());
		}

		animProps = AnimationProps(animType, animationId, frameDuration, texture, spriteBoxSequence, hitbox);
		AnimationManager::Add(animationId, animProps);
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
