#pragma once
#include "CollisionEngine.h"
#include "EntityState.h"
#include "PlayerPowerLevel.h"
#include "MarioAnimationSet.h"
#include "Direction.h"

namespace Entities {
	class Mario : public Entity {
	public:
		virtual void TakeDamage() = 0;

		Mario(
			const Utils::Vector2<float>& position, HDirection initialFacingDirection, const MarioAnimationSet& animationSet,
			const std::string& hitboxId, PlayerPowerLevel powerLevel
		);

		void OnReady() override;
		virtual void Update(float delta) override;
		virtual void Render() override;
		HDirection GetFacingDirection();
		void TurnInvinsible();
		bool IsInvincible();
		void StartReachedGoalRouletteAnimation();

		PlayerPowerLevel GetPowerLevel();

		/// <summary>
		/// Event will notify when player jump (meaning point up combo need to restart)
		/// </summary>
		Event<>& GetRestartPointUpEvent();

		void Bounce();

	protected:
		virtual void OnOutOfWorld() override;

		void NormalUpdate(float delta);
		void InvincibilityUpdate(float delta);
		void NormalRender();
		void InvincibilityRender();

		void SwitchState(EntityState<Mario>::StateHandler state);
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
		/// Keep player in world horizontally
		/// </summary>
		void ClipHorizontalPosition();

		EntityState<Mario> marioState;
		Utils::Vector2<int> dir;
		float time;
		bool onGround;
		bool runBeforeJump;
		int lastPressedKeyHorizontal;
		PlayerPowerLevel powerLevel;

	private:
		EntityState<Mario> updateState;
		float invincibleTime;
		Event<> restartPointUp;
		MarioAnimationSet animationSet;


	public:
		static const int FLASH_DURATION;

	protected:
		static const float MAX_WALK_SPEED;
		static const float MAX_RUN_SPEED;
		static const float ACCELERATION;
		static const float JUMP_SPEED;
		static const float JUMP_SPEED_AFTER_WALK;
		static const float JUMP_SPEED_RELASE_EARLY;
		static const float BOUNCE_SPEED;
		static const float BOUNCE_SPEED_HOLD_JUMP;
		static const float DEATH_JUMP_SPEED;
		static const float DEATH_FALL_ACCEL;
		static const float RUN_STATE_ANIM_SPEED;
		static const float WALK_SPEED_REACHED_GOAL_ROULETTE;
		static const float INVINCIBLE_DURATION;
	};
	typedef Mario* LPMario;
}
