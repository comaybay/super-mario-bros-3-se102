#include "MovementKnockedOver.h"
#include "EntityConstants.h"

const Utils::Vector2<float> MovementKnockedOver::KNOCK_OVER_VELOCITY(60, -250);
const float MovementKnockedOver::EXIST_DURATION = 5;

MovementKnockedOver::MovementKnockedOver(LPEntity entity, HDirection direction)
	: Movement(this, &MovementKnockedOver::Init),
	entity(entity)
{
	dir = direction == HDirection::LEFT ? -1 : 1;
}

void MovementKnockedOver::Init(float _)
{
	Utils::Vector2<float> vel = { KNOCK_OVER_VELOCITY.x * dir, KNOCK_OVER_VELOCITY.y };
	entity->SetVelocity(vel);
	SwitchState(&MovementKnockedOver::Run);
}

void MovementKnockedOver::Run(float delta)
{
	Utils::Vector2<float> vel = entity->GetVelocity();
	vel.y += EntityConstants::GRAVITY * delta;
	vel.y = min(vel.y, EntityConstants::MAX_FALL_SPEED);
	entity->SetVelocity(vel);

	time += delta;
	if (time > EXIST_DURATION)
		SetMovementFinished();
}
