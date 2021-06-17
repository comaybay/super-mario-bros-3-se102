#include "Hitbox.h"

Hitbox::Hitbox(const Utils::Vector2<float>& relativePosition, const Utils::Dimension& dimension)
	: relativePosition(relativePosition), dimension(dimension)
{}


Hitbox::Hitbox() {}
