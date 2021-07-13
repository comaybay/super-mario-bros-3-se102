#include "MovementBlockHit.h"
#include "EntityConstants.h"
using namespace Utils;

const float MovementBlockHit::UP_SPEED = 250;
const float MovementBlockHit::FALL_SPEED = EntityConstants::GRAVITY * 6;

MovementBlockHit::MovementBlockHit(LPEntity entity, const Utils::Vector2<float>& initalPosition) :
	Movement(this, &MovementBlockHit::Init),
	entity(entity),
	initialPos(initalPosition)
{
}

void MovementBlockHit::Init(float delta)
{
	entity->SetVelocity({ 0, -UP_SPEED });
	SwitchState(&MovementBlockHit::Run);
}

void MovementBlockHit::Run(float delta)
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

	SetMovementFinished();
}
