#pragma once
#include "Entity.h"
#include "Direction.h"
#include "Movement.h"

class MovementKnockedOver : public Movement<MovementKnockedOver>
{
public:
	MovementKnockedOver(LPEntity entity, HDirection direction);

private:
	void Init(float delta);
	void Run(float delta);

	LPEntity entity;
	int dir;
	float time;
	static const Utils::Vector2<float> KNOCK_OVER_VELOCITY;
	static const float EXIST_DURATION;
};

