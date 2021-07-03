#pragma once
#include "Mario.h"
#include "Utils.h"

namespace Entities {
	class MarioSmall : public Mario {
	public:
		MarioSmall(Utils::Vector2<float> position);
		void TakeDamage() override;
		void Update(float delta) override;
		void OnCollision(CollisionData data) override;
	private:
		void OnOutOfWorld() override;
		void Die(float delta);
		void DieFall(float delta);

		bool died;
		EntityState<MarioSmall> smallMarioState;
		static const Mario::AnimationSet animationSet;
	};
}