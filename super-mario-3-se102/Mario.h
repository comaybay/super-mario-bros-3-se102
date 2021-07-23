#pragma once
#include "CollisionEngine.h"
#include "EntityState.h"
#include "PlayerPowerLevel.h"
#include "MarioAnimationSet.h"
#include "Direction.h"
#include "Koopa.h"

namespace Entities {
	class Mario : public Entity {
	public:
		virtual void TakeDamage() = 0;

		Mario(
			const Utils::Vector2<float>& position, HDirection initialFacingDirection, const MarioAnimationSet& animationSet,
			const std::string& hitboxId, PlayerPowerLevel powerLevel
		);

		virtual void OnReady() override;
		virtual void Update(float delta) override;
		void PostUpdate() override;
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

		/// <summary>
		/// Applied behaviours when on a note block
		/// </summary>
		void SetOnNoteBlock(bool state);

	protected:
		bool IsBouncingUp();
		bool IsJumping();
		bool IsFalling();

		/// <summary>
		/// Keep player in camera horizontally
		/// </summary>
		void ClipHorizontalPosition();

		virtual void OnCollision(CollisionData data);
		virtual void OnOutOfWorld() override;

		virtual void UpdateAnimation();
		virtual void UpdateIdleStateAnimation();

		virtual void HandleJumpStateMovement(float delta);
		virtual void HandleFallStateMovement(float delta);

		void UpdateIncreasePowerMeter(float time);
		void UpdateDecreasePowerMeter(float time);
		void UpdateInputDirection();
		void UnsubscribeToCollisionEngine();
		void ApplyHorizontalMovement(float delta);
		void ApplyFriction(float delta);

	private:
		void NormalUpdate(float delta);
		void InvincibilityUpdate(float delta);
		void NormalRender();
		void InvincibilityRender();

		void SwitchState(EntityState<Mario>::StateHandler state);
		void Idle(float delta);
		void Walk(float delta);
		void WalkSpeedUp(float delta);
		void Run(float delta);
		void Jump(float delta);
		void Fall(float delta);
		void BounceUp(float delta);
		void ReachedGoalRouletteFall(float delta);
		void ReachedGoalRouletteWalkAway(float delta);
		void OutOfWorldDeath(float delta);
		void OnNoteBlock(float delta);
		void OffNoteBlock(float delta);

	protected:
		bool isRunning;
		bool onGround;
		int lastPressedKeyHorizontal;
		Utils::Vector2<int> inputDir;

	private:
		EntityState<Mario> state;
		std::string normalHitboxId;
		float time;
		float powerMeterTime;
		float walkAnimSpeed;
		bool instantDecrease;
		bool runBeforeJump;
		PlayerPowerLevel powerLevel;
		EntityState<Mario> updateState;
		float invincibleTime;
		Event<> restartPointUp;
		MarioAnimationSet animationSet;

	public:
		static const int FLASH_DURATION;

	protected:
		static const float MAX_WALK_SPEED;
		static const float MAX_RUN_SPEED;
		static const float WAIT_TIME_BEFORE_INCREASE_POWER_METER;
		static const float POWER_METER_INCREASE_RATE;
		static const float POWER_METER_DECREASE_RATE;
		static const float ACCELERATION;
		static const float FRICTION_ACCEL;
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
		static const float MAX_MOVE_ANIM_SPEED;
		static const float INCREASE_MOVE_ANIM_UNIT;
	};
	typedef Mario* LPMario;
}
