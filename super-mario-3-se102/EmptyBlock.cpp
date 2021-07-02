#include "EmptyBlock.h"
#include "EntityConstants.h"
#include "Group.h"
using namespace Entities;

const float EmptyBlock::UP_SPEED = 250;
const float EmptyBlock::FALL_SPEED = EntityConstants::GRAVITY * 3;


EmptyBlock::EmptyBlock(const Utils::Vector2<float>& position)
	: Entity::Entity(
		position,
		"EmptyBlock",
		HitboxId::TILE_SIZE_HITBOX,
		{ "EmptyBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	state(EntityState<EmptyBlock>(this, &EmptyBlock::HitAnimation))
{
	initialPos = position;
	velocity.y = -UP_SPEED;
}

void EmptyBlock::Update(float delta)
{
	Entity::Update(delta);
	state.Handle(delta);
}

void EmptyBlock::HitAnimation(float delta) {
	velocity.y += FALL_SPEED * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	if (position.y > initialPos.y) {
		velocity = { 0, 0 };
		position = initialPos;
		state.SetHandler(&EmptyBlock::Idle);
	}
}


void EmptyBlock::Idle(float _) {
}