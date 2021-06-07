#include "Entities.h"
#include "Groups.h"
using namespace Entities;

//TODO: COLLISION BLOCKS AS CONST STRING
CollisionWallType1::CollisionWallType1(Utils::Vector2 position, Utils::Dimension dimension)
	: Entity::Entity(position, AnimationId::NONE, { Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_1 }), dim(dimension)
{
}

Utils::Dimension CollisionWallType1::GetDimension()
{
	return dim;
}

