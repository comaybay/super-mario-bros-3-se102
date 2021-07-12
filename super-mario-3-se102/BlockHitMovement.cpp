#include "BlockHitMovement.h"
#include "EntityConstants.h"
using namespace Utils;

const float BlockHitMovement::UP_SPEED = 250;
const float BlockHitMovement::FALL_SPEED = EntityConstants::GRAVITY * 6;

BlockHitMovement::BlockHitMovement(LPEntity entity, const Utils::Vector2<float>& initalPosition) :
	entity(entity),
	initialPos(initalPosition),
	movementFinished(false),
	handler(&BlockHitMovement::Init)
{
}

void BlockHitMovement::Update(float delta)
{
	if (!movementFinished)
		(this->*handler)(delta);
}

bool BlockHitMovement::Finished() {
	return movementFinished;
}

void BlockHitMovement::Reset()
{
	handler = &BlockHitMovement::Init;
	movementFinished = false;
}

void BlockHitMovement::Init(float delta)
{
	entity->SetVelocity({ 0, -UP_SPEED });
	handler = &BlockHitMovement::Run;
}

void BlockHitMovement::Run(float delta)
{
	if (entity->GetPosition().y <= initialPos.y) {
		Vector2<float> vel = entity->GetVelocity();
		vel.y += FALL_SPEED * delta;
		vel.y = min(vel.y, EntityConstants::MAX_FALL_SPEED);
		entity->SetVelocity(vel);
		return;
	}

	entity->SetVelocity({ 0, 0 });
	entity->SetPosition(initialPos);
	movementFinished = true;
}
