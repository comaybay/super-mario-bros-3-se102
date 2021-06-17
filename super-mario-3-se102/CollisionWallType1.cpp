#include "CollisionWallType1.h"
#include "Groups.h"
using namespace Entities;

CollisionWallType1::CollisionWallType1(Utils::Vector2<float> position, Utils::Dimension dimension)
	: Entity::Entity(
		position,
		AnimationId::NONE,
		{ Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_1 },
		GridType::WALL_ENTITIES
	),
	dim(dimension)
{
}

Utils::Dimension CollisionWallType1::GetDimension()
{
	return dim;
}

