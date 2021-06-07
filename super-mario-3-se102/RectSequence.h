#pragma once
#include <string>
#include "Utils.h"
class RectSequence
{
private:
	int GetSize();
	void Get(int index);

public:
	Utils::Dimension dim;
	Utils::Vector2 position;
	int size;
};

