#pragma once
#include "CollisionEngine.h"
#include "EntityState.h"

namespace Entities {
	class GoalRoulette : public Entity
	{
	public:
		GoalRoulette(const Utils::Vector2<float>& position);
		void OnReady() override;
		void Update(float delta) override;

	private:
		void Idle(float _);
		void FlyUp(float delta);

		void OnCollision(CollisionData data);

		EntityState<GoalRoulette> state;
		float time;
		static const float UP_SPEED;
		static const float FLY_UP_EXIST_DURATION;
	};
}

