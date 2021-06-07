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
	if (i >= size - 1) //-1 null terminate
		return 4095; //FFF

	//convert 3 digits hex to int
	return Utils::HexCharToInt(ground[i]) * 16 * 16 +
		Utils::HexCharToInt(ground[i + 1]) * 16 +
		Utils::HexCharToInt(ground[i + 2]);

}
