#pragma once
#include "CollisionEngine.h"
#include "EntityState.h"
#include "PlayerPowerLevel.h"
#include "MarioAnimationSet.h"

namespace Entities {
	class Mario : public Entity {
	public:


		virtual void TakeDamage() = 0;

		Mario(const Utils::Vector2<float>& position, const MarioAnimationSet& animationSet, PlayerPowerLevel powerLevel);
		void OnReady() override;
		virtual void Update(float delta) override;
		void StartReachedGoalRouletteAnimation();

		PlayerPowerLevel GetPowerLevel();

		/// <summary>
		/// Event will notify when mario jump (meaning point up combo need to restart)
		/// </summary>
		Event<>& GetRestartPointUpEvent();

		void Bounce();

	protected:
		virtual void OnOutOfWorld() override;

		void SwitchState(EntityState<Mario>::Handler state);
		void Idle(float delta);
		void Walk(float delta);
		void Run(float delta);
		void Jump(float delta);
		void Fall(float delta);
		void BounceUp(float delta);
		void ReachedGoalRouletteFall(float delta);
		void ReachedGoalRouletteWalkAway(float delta);
		void OutOfWorldDeath(float delta);

		virtual void OnCollision(CollisionData data);
		void UpdateHorizontalDirection();
		void UnsubscribeToCollisionEngine();

		void ApplyHorizontalMovement(float delta);
		void ApplyFriction(float delta);
		/// <summary>
		/// Keep mario in world horizontally
		/// </summary>
		void ClipHorizontalPosition();

		EntityState<Mario> marioState;
		Utils::Vector2<float> dir;
		float time;
		bool onGround;
		bool runBeforeJump;
		int lastPressedKeyHorizontal;
		PlayerPowerLevel powerLevel;
		static const float MAX_WALK_SPEED;
		static const float MAX_RUN_SPEED;
		static const Utils::Vector2<float> ACCELERATION;
		static const float JUMP_SPEED;
		static const float JUMP_SPEED_AFTER_MAX_WALK_SPEED;
		static const float JUMP_SPEED_RELASE_EARLY;
		static const float BOUNCE_SPEED;
		static const float BOUNCE_SPEED_HOLD_JUMP;
		static const float DEATH_JUMP_SPEED;
		static const float DEATH_FALL_ACCEL;
		static const float RUN_STATE_ANIM_SPEED;
		static const float WALK_SPEED_REACHED_GOAL_ROULETTE;

	private:
		Event<> restartPointUp;
		EventHandler<Mario, CollisionData> onCollision;
		MarioAnimationSet animationSet;
	};
	typedef Mario* LPMario;
}
