#include "CollisionWallType1.h"
#include "Group.h"
using namespace Entities;
using namespace Utils;

CollisionWallType1::CollisionWallType1(const Vector2<float>& position, const Dimension<int>& dimension)
	: Entity::Entity(
		position,
		{ Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::WALL_ENTITIES
	),
	hitbox(Hitbox(Vector2<float>(0, 0), dimension))
{}

LPConstHitbox CollisionWallType1::GetHitbox()
{
	return &hitbox;
}

Dimension<int> CollisionWallType1::GetCurrentSpriteDimension()
{
	return hitbox.GetDimension();
}

