#include "EmptyBlock.h"
#include "EntityConstants.h"
#include "Group.h"
using namespace Entities;

EmptyBlock::EmptyBlock(const Utils::Vector2<float>& position)
	: Entity::Entity(
		position,
		"EmptyBlock",
		HitboxId::TILE_SIZE_HITBOX,
		{ "EmptyBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	state(EntityState<EmptyBlock>(this, &EmptyBlock::HitAnimation)),
	blockHitMovement(new BlockHitMovement(this, position))
{
}

void EmptyBlock::Update(float delta)
{
	Entity::Update(delta);
	state.Handle(delta);
}

void EmptyBlock::HitAnimation(float delta) {
	bool done = blockHitMovement->Update(delta);
	if (done) {
		delete blockHitMovement;
		blockHitMovement = nullptr;
		state.SetHandler(&EmptyBlock::Idle);
	}
}


void EmptyBlock::Idle(float _) {
}