#include "MovementKnockOver.h"

const Utils::Vector2<float> MovementKnockOver::KNOCK_OVER_VELOCITY(60, -250);
const float MovementKnockOver::EXIST_DURATION = 5;

MovementKnockOver::MovementKnockOver(LPEntity entity, HDirection direction)
	: Movement(this, &MovementKnockOver::Init),
	entity(entity)
{
	dir = direction == HDirection::LEFT ? -1 : 1;
}

void MovementKnockOver::Init(float _)
{
	Utils::Vector2<float> vel = { KNOCK_OVER_VELOCITY.x * dir, KNOCK_OVER_VELOCITY.y };
	entity->SetVelocity(vel);
	SwitchState(&MovementKnockOver::Run);
}

void MovementKnockOver::Run(float delta)
{
	time += delta;
	if (time > EXIST_DURATION)
		SetMovementFinished();
}
