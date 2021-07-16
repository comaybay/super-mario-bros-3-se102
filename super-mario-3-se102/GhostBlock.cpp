#include "GhostBlock.h"
#include "Group.h"

using namespace Entities;

GhostBlock::GhostBlock(const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, HitboxId::TILE_SIZE_HITBOX, { "GhostBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 }, GridType::WALL_ENTITIES)
{
}
