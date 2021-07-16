#include "EmptyBlock.h"
#include "EntityConstants.h"
#include "Group.h"
using namespace Entities;

EmptyBlock::~EmptyBlock()
{
	delete blockHitMovement;
}

EmptyBlock::EmptyBlock(const Utils::Vector2<float>& position)
	: Entity::Entity(
		position,
		"EmptyBlock",
		HitboxId::TILE_SIZE_HITBOX,
		{ "EmptyBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	state(EntityState<EmptyBlock>(this, &EmptyBlock::HitAnimation)),
	blockHitMovement(new MovementBlockHit(this, Direction::UP, position))
{
}

void EmptyBlock::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void EmptyBlock::HitAnimation(float delta) {
	blockHitMovement->Update(delta);
	if (blockHitMovement->Finished()) {
		delete blockHitMovement;
		blockHitMovement = nullptr;
		state.SetState(&EmptyBlock::Idle);
	}
}

void EmptyBlock::Idle(float _) {
}