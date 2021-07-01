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
			AnimationSet(
				const std::string& idleLeft,
				const std::string& idleRight,
				const std::string& turnLeft,
				const std::string& turnRight,
				const std::string& walkLeft,
				const std::string& walkRight,
				const std::string& jumpLeft,
				const std::string& jumpRight
			);
		};

		virtual void TakeDamage() = 0;

		Mario(const Utils::Vector2<float>& position, const AnimationSet& animationSet);
		void OnReady() override;
		virtual void Update(float delta) override;

		/// <summary>
		/// Event will notify when mario jump (meaning point up combo need to restart)
		/// </summary>
		Event<>& GetRestartPointUpEvent();

		void SwitchState(EntityState<Mario>::Handler state);
		void Idle(float delta);
		void Walk(float delta);
		void Run(float delta);
		void Jump(float delta);
		void Fall(float delta);
		void Bounce(float delta);

	protected:
		void OnCollision(CollisionData data);
		void UpdateHorizontalDirection();
		void UnsubscribeToCollisionEngine();

		void ApplyHorizontalMovement(float delta);
		void ApplyFriction(float delta);

		EntityState<Mario> marioState;
		Utils::Vector2<float> dir;
		float time;
		bool onGround;
		bool runBeforeJump;
		int lastPressedKeyHorizontal;
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

	private:
		Event<> restartPointUp;
		EventHandler<Mario, CollisionData> onCollision;
		AnimationSet animationSet;
	};
}