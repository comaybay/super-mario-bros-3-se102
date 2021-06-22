#pragma once
#include "Utils.h"
#include <windows.h>

struct SpriteBox {
	RECT rect;
	Utils::Vector2<int> offset;
	SpriteBox(RECT rect, Utils::Vector2<int> offset);
};
