#pragma once
#include "Mario.h"
#include "Utils.h"

namespace Entities {
	class MarioSmall : public Mario {
	public:
		MarioSmall(Utils::Vector2<float> position, HDirection initialFacingDirection);
		void TakeDamage() override;
		void Update(float delta) override;
		void OnCollision(CollisionData data) override;

	private:
		void UpdateIdleStateAnimation() override;

		void OnOutOfWorld() override;
		void Die(float delta);
		void DieFall(float delta);

		float time;
		bool died;
		EntityState<MarioSmall> smallMarioState;
		static const MarioAnimationSet animationSet;
	};
}