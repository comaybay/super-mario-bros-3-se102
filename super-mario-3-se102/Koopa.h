#pragma once
#include "Entity.h"
#include "Utils.h"
#include "EntityState.h"
#include "CollisionEngine.h"
#include "IKnockedOverable.h"
#include "MovementKnockedOver.h"
#include "Mario.h"

namespace Entities {
	class Koopa : public Entity, public IKnockedOverable
	{
	public:
		~Koopa();
		Koopa(const std::string& colorType, const Utils::Vector2<float>& position);
		void Update(float delta) override;
		void GetKnockedOver(HDirection direction) override;
		bool IsSliding();

	private:
		void OnReady() override;
		void OnCollision(CollisionData data);
		void OnPlayerReleaseHold(LPEntity player);
		void HandleWallCollision(const CollisionData& data);
		void HandlePlayerCollision(const CollisionData& data);

		void SwitchState(EntityState<Koopa>::StateHandler handler);
		void MoveAround(float delta);
		void ShellIdle(float delta);
		void ShellSlide(float delta);
		void KnockedOver(float delta);
		void Hold(float delta);

		std::string colorCode;
		std::string colorType;
		LPMovement<MovementKnockedOver> knockedOverMovement;
		EntityState<Koopa> state;
		Utils::Vector2<float> prevOnGroundPosition;
		LPMario holder;
		bool onGround;
		bool lock;
	public:
		static const float WALK_SPEED;
		static const float SHELL_SLIDE_SPEED;
		static const float FRICTION;
	};
	typedef Koopa* LPKoopa;
}
