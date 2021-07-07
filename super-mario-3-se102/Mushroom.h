#pragma once
#include "EntityState.h"
#include "Entity.h"
#include "CollisionEngine.h"

namespace Entities {
	class Mushroom : public Entity
	{
	public:
		Mushroom(const Utils::Vector2<float>& position);
		void Update(float delta) override;
		void OnReady() override;

	private:
		void OnCollision(CollisionData data);
		void HandleWallCollision(const CollisionData& data);
		void HandlePlayerCollision(const CollisionData& data);

		void Wait(float delta);
		void RiseUp(float delta);
		void Move(float delta);

		EntityState<Mushroom> state;
		float time;
		float riseDestY;
		static const float WAIT_TIME;
		static const float RISE_SPEED;
		static const float MOVE_SPEED;
		static const float FALL_SPEED;
	};
}