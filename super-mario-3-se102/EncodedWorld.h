#pragma once
#include <string>

class EncodedWorld {
public:
	EncodedWorld(int size, int width, char* foreground, char* background, const std::string& textureId);
	~EncodedWorld();
	int GetBackgroundIndex(int x, int y);
	int GetForegroundIndex(int x, int y);
	const std::string& GetTextureId();

private:
	int GetIndex(char* ground, int x, int y);
	int HexCharToInt(const char& ch);

	char* foreground;
	char* background;
	int width;
	int size;
	std::string textureId;
};
typedef EncodedWorld* LPEncodedWorld;