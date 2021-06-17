#pragma once
#include "Utils.h"

class Hitbox {
public:
	/// <summary>
	/// relativePosition: position relative to the position of entity
	/// </summary>
	Hitbox(const Utils::Vector2<float>& relativePosition, const Utils::Dimension& dimension);
	Hitbox();

	const Utils::Vector2<float>& GetRelativePosition() const;
	const Utils::Dimension& GetDimension() const;
private:
	Utils::Vector2<float> relativePosition;
	Utils::Dimension dimension;
};
typedef Hitbox* LPHitbox;
typedef const Hitbox* LPConstHitbox;

class HitboxId {
public:
	static const std::string TILE_SIZE_HITBOX;
	static const std::string NONE;
};