#pragma once
#include "CollisionEngine.h"
#include "Direction.h"

namespace Entities {
	class MarioSpinAttack : public Entity
	{
	public:
		MarioSpinAttack(LPEntity player, HDirection dir);
		void OnReady() override;
		void Update(float delta) override;

	private:
		void OnCollision(CollisionData data);
		void OnPlayerDestroy(LPEntity _);

		LPEntity player;
		float offsetPosX;
		float hitboxOffsetX;

		static const float SPEED;
		static const float MAX_DISTANCE;
	};
}