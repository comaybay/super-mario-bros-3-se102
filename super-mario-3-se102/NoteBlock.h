#pragma once
#include "EntityState.h"
#include "Entity.h"
#include "MovementBlockHit.h"
#include "CollisionEngine.h"
#include "Mario.h"
#include "IKnockedOverable.h"

namespace Entities {
	class NoteBlock : public Entity, public IKnockedOverable
	{
	public:
		NoteBlock(std::string contentId, const Utils::Vector2<float>& position);
		void OnReady() override;
		void Update(float delta) override;

		void GetKnockedOver(HDirection direction);

	private:
		void OnCollision(CollisionData data);
		void OnPlayerDestroy(LPEntity _);
		void ExposeContent(LPMario player);

		void Idle(float delta);
		void Hit(float delta);
		void StompedOn(float delta);

		EntityState<NoteBlock> state;
		LPMario targetPlayer;
		bool pressedJump;

		MovementBlockHit blockHitMovementUp;
		MovementBlockHit blockHitMovementDown;

		std::string contentId;
	};
}


