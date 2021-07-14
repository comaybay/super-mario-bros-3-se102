#pragma once
#include "EntityState.h"
#include "Entity.h"
#include "MovementBlockHit.h"
#include "CollisionEngine.h"
#include "Mario.h"

namespace Entities {
	class SuperNoteBlock : public Entity
	{
	public:
		SuperNoteBlock(const Utils::Vector2<float>& position);
		void OnReady() override;
		void Update(float delta) override;
	private:
		void OnCollision(CollisionData data);

		void Idle(float delta);
		void Hit(float delta);
		void StompedOn(float delta);

		EntityState<SuperNoteBlock> state;
		LPMario targetPlayer;
		bool pressedJump;

		MovementBlockHit blockHitMovementUp;
		MovementBlockHit blockHitMovementDown;
	};
}


