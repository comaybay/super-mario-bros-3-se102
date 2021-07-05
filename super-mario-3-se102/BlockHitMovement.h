#pragma once
#include "Entity.h"
#include "Utils.h"

class BlockHitMovement
{
public:
	BlockHitMovement(LPEntity entity, const Utils::Vector2<float>& initalPosition);
	/// <summary>
	/// Returns true if finished movement
	/// </summary>
	bool Update(float delta);

	void Reset();


private:
	void Init(float delta);
	void Run(float delta);

	void(BlockHitMovement::* handler)(float);
	LPEntity entity;
	Utils::Vector2<float> initialPos;
	bool movementFinished;
	static const float UP_SPEED;
	static const float FALL_SPEED;
};

typedef BlockHitMovement* LPBlockHitMovement;