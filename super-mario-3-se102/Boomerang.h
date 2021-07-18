#pragma once
#include "CollisionEngine.h"
#include "Direction.h"
#include "EntityState.h"

namespace Entities {
	class Boomerang : public Entity
	{
	public:
		Boomerang(LPEntity holder, const Utils::Vector2<float>& offset, HDirection direction);
		void OnReady() override;
		void Update(float delta) override;

		void StartMoving();
		void SetDirection(HDirection direction);
		void SetOffset(const Utils::Vector2<float> offset);
	private:
		void OnCollision(CollisionData data);

		void Idle(float delta);
		void MoveForward(float delta);
		void TurnBack(float delta);

		EntityState<Boomerang> state;
		LPEntity holder;
		HDirection dir;
		float endY;
		float startX;
		bool thrown;
		Utils::Vector2<float> offset;

		static const float OFFSET_END_Y;
		static const float MOVE_SPEED;
		static const float ACCELERATION;
		static const float UP_SPEED;
		static const float FALL_SPEED;
		static const float DISTANCE_BEFORE_TURN_BACK;
	};
	typedef Boomerang* LPBoomerang;
}

