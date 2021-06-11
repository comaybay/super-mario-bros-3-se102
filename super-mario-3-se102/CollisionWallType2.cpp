#include "Entities.h"
#include "Groups.h"
using namespace Entities;

CollisionWallType2::CollisionWallType2(Vector2<float> position, Dimension dimension)
	: Entity::Entity(
		position,
		AnimationId::NONE,
		{ Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_2 },
		GridType::WALL_ENTITIES
	),
	dim(dimension)
{
}

Dimension CollisionWallType2::GetDimension() {
	return dim;
};
