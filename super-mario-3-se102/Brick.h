#pragma once
#include "MovementBlockHit.h"
#include "EntityState.h"
#include "CollisionEngine.h"
#include "Mario.h"
#include "IKnockedOverable.h"

namespace Entities {
	class Brick : public Entity, public IKnockedOverable {
	public:
		Brick(const std::string& contentId, const Utils::Vector2<float>& position);
		void OnReady() override;
		void Update(float delta) override;
		void GetKnockedOver(HDirection direction) override;

	private:
		void ExposeContent(LPMario player);
		void OnCollision(CollisionData data);

		void Idle(float delta);
		void Hit(float delta);
		EntityState<Brick> state;

		MovementBlockHit blockHitMovement;
		std::string contentId;
	};
}

