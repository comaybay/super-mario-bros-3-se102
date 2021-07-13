#pragma once
#include "MovementBlockHit.h"
#include "EntityState.h"
#include "CollisionEngine.h"

namespace Entities {
	class Brick : public Entity {
	public:
		Brick(LPEntity content, const Utils::Vector2<float>& position);
		void OnReady() override;
		void Update(float delta) override;
	private:
		void ExposeContent();
		void OnCollision(CollisionData data);

		void Idle(float delta);
		void Hit(float delta);
		EntityState<Brick> state;

		MovementBlockHit blockHitMovement;
		LPEntity content;
	};
}

