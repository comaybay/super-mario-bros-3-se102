#include "Hitbox.h"

const std::string HitboxId::TILE_SIZE_HITBOX = "__HITBOX_TILE_SIZE__";
const std::string HitboxId::NONE = "__HITBOX_NONE__";

Hitbox::Hitbox(const Utils::Vector2<float>& relativePosition, const Utils::Dimension& dimension)
	: relativePosition(relativePosition), dimension(dimension)
{}

Hitbox::Hitbox() {}

const Utils::Vector2<float>& Hitbox::GetRelativePosition() const
{
	return relativePosition;
}

const Utils::Dimension& Hitbox::GetDimension() const
{
	return dimension;
}
