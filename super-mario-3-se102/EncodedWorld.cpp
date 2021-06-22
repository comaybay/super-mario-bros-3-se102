#include "EncodedWorld.h"
#include "Utils.h"

EncodedWorld::EncodedWorld(int size, int width, char* background, char* foreground)
	: size(size), width(width), background(background), foreground(foreground)
{}

EncodedWorld::~EncodedWorld()
{
	delete foreground;
	delete background;
}

int EncodedWorld::GetBackgroundIndex(int x, int y) {
	return GetIndex(background, x, y);
}

int EncodedWorld::GetForegroundIndex(int x, int y) {
	return GetIndex(foreground, x, y);
}

int EncodedWorld::GetIndex(char* ground, int x, int y)
{
	int i = x * 3 + y * width;
	if (i >= size - 1) //-1 null terminate char
		return 4095; //FFF

	//convert 3 digits hex to int
	return
		HexCharToInt(ground[i]) * 16 * 16 +
		HexCharToInt(ground[i + 1]) * 16 +
		HexCharToInt(ground[i + 2]);

}

int EncodedWorld::HexCharToInt(const char& ch) {
	if (ch >= '0' && ch <= '9')
		return ch - '0';

	if (ch >= 'A' && ch <= 'F')
		return ch - 'A' + 10;

	if (ch >= 'a' && ch <= 'f')
		return ch - 'a' + 10;

	throw std::exception("charToInt failed: invaild hex char");
}