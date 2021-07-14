#include "MovementBlockHit.h"
#include "EntityConstants.h"
using namespace Utils;

const float MovementBlockHit::FORWARD_SPEED = 250;
const float MovementBlockHit::BACKWARD_SPEED = EntityConstants::GRAVITY * 6;

MovementBlockHit::MovementBlockHit(LPEntity entity, Direction direction, const Utils::Vector2<float>& initalPosition) :
	Movement(this, &MovementBlockHit::Init),
	entity(entity),
	direction(direction),
	initialPos(initalPosition)
{
}

void MovementBlockHit::Init(float delta)
{
	switch (direction) {
	case Direction::LEFT:
		entity->SetVelocity({ -FORWARD_SPEED, 0 });
		SwitchState(&MovementBlockHit::MoveLeft);
		break;
	case Direction::RIGHT:
		entity->SetVelocity({ FORWARD_SPEED, 0 });
		SwitchState(&MovementBlockHit::MoveRight);
		break;
	case Direction::UP:
		entity->SetVelocity({ 0, -FORWARD_SPEED });
		SwitchState(&MovementBlockHit::MoveUp);
		break;
	case Direction::DOWN:
		entity->SetVelocity({ 0, FORWARD_SPEED });
		SwitchState(&MovementBlockHit::MoveDown);
		break;
	}
}

void MovementBlockHit::MoveLeft(float delta) {
	if (entity->GetPosition().x <= initialPos.x) {
		Vector2<float> vel = entity->GetVelocity();
		vel.x += BACKWARD_SPEED * delta;
		entity->SetVelocity(vel);
		return;
	}

	entity->SetVelocity({ 0, 0 });
	entity->SetPosition(initialPos);

	SetMovementFinished();
}

void MovementBlockHit::MoveRight(float delta) {
	if (entity->GetPosition().x >= initialPos.x) {
		Vector2<float> vel = entity->GetVelocity();
		vel.x -= BACKWARD_SPEED * delta;
		entity->SetVelocity(vel);
		return;
	}

	entity->SetVelocity({ 0, 0 });
	entity->SetPosition(initialPos);

	SetMovementFinished();
}

void MovementBlockHit::MoveUp(float delta) {
	if (entity->GetPosition().y <= initialPos.y) {
		Vector2<float> vel = entity->GetVelocity();
		vel.y += BACKWARD_SPEED * delta;
		entity->SetVelocity(vel);
		return;
	}

	entity->SetVelocity({ 0, 0 });
	entity->SetPosition(initialPos);

	SetMovementFinished();
}

void MovementBlockHit::MoveDown(float delta) {
	if (entity->GetPosition().y >= initialPos.y) {
		Vector2<float> vel = entity->GetVelocity();
		vel.y -= BACKWARD_SPEED * delta;
		entity->SetVelocity(vel);
		return;
	}

	entity->SetVelocity({ 0, 0 });
	entity->SetPosition(initialPos);

	SetMovementFinished();
}
