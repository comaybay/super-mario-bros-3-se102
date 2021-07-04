#pragma once
#include <string>
#include "Utils.h"
class RectSequence
{
private:
	int GetSize();
	void Get(int index);

public:
	Utils::Dimension<float> dim;
	Utils::Vector2<float> position;
	int size;
};

