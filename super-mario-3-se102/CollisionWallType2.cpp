#include "CollisionWallType2.h"
#include "Group.h"
using namespace Entities;
using namespace Utils;

CollisionWallType2::CollisionWallType2(Vector2<float> position, Dimension<int> dimension)
	: Entity::Entity(
		position,
		{ Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_2 },
		GridType::WALL_ENTITIES
	),
	hitbox(Hitbox(Vector2<float>(0, 0), dimension))
{
}

const Hitbox& CollisionWallType2::GetHitbox()
{
	return hitbox;
}

Dimension<int> CollisionWallType2::GetCurrentSpriteDimension()
{
	return hitbox.GetDimension();
}


