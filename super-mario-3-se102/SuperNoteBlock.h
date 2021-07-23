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
		SuperNoteBlock(const std::string& scenePath, const Utils::Vector2<float>& position);
		void OnReady() override;
		void Update(float delta) override;
	private:
		void OnCollision(CollisionData data);

		void Idle(float delta);
		void Hit(float delta);
		void StompedOn(float delta);
		void PrepareSwitchScene(float delta);

		std::string scenePath;
		EntityState<SuperNoteBlock> state;
		LPMario targetPlayer;
		bool pressedJump;
		float time;

		MovementBlockHit blockHitMovementUp;
		MovementBlockHit blockHitMovementDown;

		static const float WAIT_DURATION;
	};
}


