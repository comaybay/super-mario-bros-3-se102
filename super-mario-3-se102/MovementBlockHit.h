#pragma once
#include "Entity.h"
#include "Utils.h"
#include "Movement.h"
#include "Direction.h"

class MovementBlockHit : public Movement<MovementBlockHit>
{
public:
	MovementBlockHit(LPEntity entity, Direction direction, const Utils::Vector2<float>& initalPosition);

private:
	void Init(float delta);
	void MoveLeft(float delta);
	void MoveRight(float delta);
	void MoveUp(float delta);
	void MoveDown(float delta);

	LPEntity entity;
	Direction direction;
	Utils::Vector2<float> initialPos;
	static const float FORWARD_SPEED;
	static const float BACKWARD_SPEED;
};

