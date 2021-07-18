#pragma once
#include "CollisionEngine.h"
#include "IKnockedOverable.h"
#include "EntityState.h"
#include "Direction.h"
#include "MovementKnockedOver.h"
#include "Boomerang.h"

namespace Entities {
	class BoomerangBrother : public Entity, public IKnockedOverable
	{
	public:
		BoomerangBrother(const Utils::Vector2<float>& position);
		void OnReady() override;
		void Update(float delta) override;

		void GetKnockedOver(HDirection direction) override;

	private:
		void OnCollision(CollisionData data);
		void UpdateAnimation();
		void UpdateAnimationPrepareThrow();
		LPEntity TryFindPlayer();
		void OnPlayerDestroy(LPEntity _);

		void MoveLeft(float delta);
		void MoveRight(float delta);
		void MoveAndThrowLeft(float delta);
		void MoveAndThrowRight(float delta);

		void Wait(float delta);
		void StompedOn(float delta);
		void KnockedOver(float delta);

		EntityState<BoomerangBrother> state;
		LPMovement<MovementKnockedOver> knockedOverMovement;
		LPEntity targetPlayer;
		LPBoomerang boomerang;
		float time;
		int turnCount;
		float hasThrownThisTurn;
		float hasTryJumpThisTurn;
		HDirection moveDir;
		const float stopXMoveLeft;
		const float stopXMoveRight;
		const float prepareThrowX;

		static const int NUMBER_OF_NORMAL_TURNS;
		static const int NUMBER_OF_THROW_TURNS;
		static const float DURATION_TILL_THROW;
		static const float WAIT_DURATION;
		static const float MOVE_SPEED;
		static const float JUMP_SPEED;
		static const int JUMP_CHANCE;
		static const Utils::Vector2<float> BOOMERANG_OFFSET_LEFT;
		static const Utils::Vector2<float> BOOMERANG_OFFSET_RIGHT;
	};
}
