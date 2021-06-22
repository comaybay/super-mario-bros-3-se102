#pragma once

class EncodedWorld {
private:
	char* foreground;
	char* background;
	int width;
	int size;

public:
	EncodedWorld(int size, int width, char* foreground, char* background);
	~EncodedWorld();
	int GetBackgroundIndex(int x, int y);
	int GetForegroundIndex(int x, int y);

private:
	int GetIndex(char* ground, int x, int y);
	int HexCharToInt(const char& ch);
};
typedef EncodedWorld* LPEncodedWorld;