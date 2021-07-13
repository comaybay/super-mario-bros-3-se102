#include "MovementKnockedOver.h"

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
	time += delta;
	if (time > EXIST_DURATION)
		SetMovementFinished();
}
