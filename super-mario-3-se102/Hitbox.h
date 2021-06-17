#pragma once
#include "Utils.h"

struct Hitbox {
	Utils::Vector2<float> relativePosition;
	Utils::Dimension dimension;
	/// <summary>
	/// relativePosition: position relative to the position of entity
	/// </summary>
	Hitbox(const Utils::Vector2<float>& relativePosition, const Utils::Dimension& dimension);
	Hitbox();
};