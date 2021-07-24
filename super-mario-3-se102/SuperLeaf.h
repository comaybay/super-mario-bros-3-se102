#pragma once
#include "CollisionEngine.h"
#include "EntityState.h"

namespace Entities {
	class SuperLeaf : public Entity
	{
	public:
		SuperLeaf(const Utils::Vector2<float>& position);
		void Update(float delta) override;
		void OnReady() override;

	private:
		void OnCollision(CollisionData data);

		void FlyUp(float delta);
		void Fall(float delta);


		EntityState<SuperLeaf> state;
		float dir;
		float time;
		float midX;

		static float MOVE_SPEED;
		static float ACCEL;
		static float UP_SPEED;
		static float FALL_SPEED;
		static float MAX_FALL_SPEED;
	};
}

