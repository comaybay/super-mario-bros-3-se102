#include "CollisionWallType1.h"
#include "Groups.h"
using namespace Entities;
using namespace Utils;

CollisionWallType1::CollisionWallType1(Vector2<float> position, Dimension dimension)
	: Entity::Entity(
		position,
		AnimationId::NONE,
		{ Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_1 },
		GridType::WALL_ENTITIES
	),
	hitbox(Hitbox(Vector2<float>(0, 0), dimension))
{}

const Hitbox& CollisionWallType1::GetHitbox()
{
	return hitbox;
}

Dimension CollisionWallType1::GetCurrentSpriteDimension()
{
	return hitbox.dimension;
}

