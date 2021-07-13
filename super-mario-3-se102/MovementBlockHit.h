#pragma once
#include "Entity.h"
#include "Utils.h"
#include "Movement.h"

class MovementBlockHit : public Movement<MovementBlockHit>
{
public:
	MovementBlockHit(LPEntity entity, const Utils::Vector2<float>& initalPosition);

private:
	void Init(float delta);
	void Run(float delta);

	LPEntity entity;
	Utils::Vector2<float> initialPos;
	static const float UP_SPEED;
	static const float FALL_SPEED;
};

