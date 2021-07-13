#pragma once
#include "Entity.h"
#include "Utils.h"

class MovementBlockHit
{
public:
	MovementBlockHit(LPEntity entity, const Utils::Vector2<float>& initalPosition);
	void Update(float delta);
	bool Finished();
	void Reset();

private:
	void Init(float delta);
	void Run(float delta);

	void(MovementBlockHit::* handler)(float);
	LPEntity entity;
	Utils::Vector2<float> initialPos;
	bool movementFinished;
	static const float UP_SPEED;
	static const float FALL_SPEED;
};

typedef MovementBlockHit* LPBlockHitMovement;
