#pragma once
#include "Entity.h"
#include "Utils.h"
#include "CollisionEngine.h"
#include "EntityState.h"

namespace Entities {
	class Mario : public Entity {
	public:
		struct AnimationSet {
			std::string idleLeft;
			std::string idleRight;
			std::string turnLeft;
			std::string turnRight;
			std::string walkLeft;
			std::string walkRight;
			std::string jumpLeft;
			std::string jumpRight;
			static const std::string DEATH;
		};

		enum class PowerLevel {
			SMALL,
			BIG,
		};

		Mario(Utils::Vector2<float> position);
		void OnReady() override;
		void Update(float delta) override;
		void SetPowerLevel(Mario::PowerLevel level);
		void TakeDamage();
		/// <summary>
		/// Event will notify when mario jump (meaning point up combo need to restart)
		/// </summary>
		Event<LPEntity>& GetRestartPointUpEvent();

		void SwitchState(EntityState<Mario>::Handler state);
		void Idle(float delta);
		void Walk(float delta);
		void Run(float delta);
		void Jump(float delta);
		void Fall(float delta);
		void Bounce(float delta);
		void Die(float delta);
		void DieFall(float delta);

	private:
		void OnCollision(CollisionData data);
		void UpdateHorizontalDirection();
		Mario::AnimationSet GetAnimationSetByPowerLevel(Mario::PowerLevel powerLevel);

		void ApplyHorizontalMovement(float delta);
		void ApplyFriction(float delta);

		Event<LPEntity> restartPointUp;
		EventHandler<Mario, CollisionData> onCollision;
		EntityState<Mario> state;
		Utils::Vector2<float> dir;
		float time;
		bool onGround;
		bool runBeforeJump;
		int lastPressedKeyHorizontal;
		Mario::PowerLevel powerLevel;
		Mario::AnimationSet animationSet;
		static const float MAX_FALL_SPEED;
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
	};
}