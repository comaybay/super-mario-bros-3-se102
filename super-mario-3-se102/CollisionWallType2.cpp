#include "CollisionWallType2.h"
#include "Groups.h"
using namespace Entities;
using namespace Utils;

CollisionWallType2::CollisionWallType2(Vector2<float> position, Dimension dimension)
	: Entity::Entity(
		position,
		AnimationId::NONE,
		{ Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_2 },
		GridType::WALL_ENTITIES
	),
	hitbox(Hitbox(Vector2<float>(0, 0), dimension))
{
}

const Hitbox& CollisionWallType2::GetHitbox()
{
	return hitbox;
}

Dimension CollisionWallType2::GetCurrentSpriteDimension()
{
	return hitbox.dimension;
}

