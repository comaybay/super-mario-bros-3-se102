#pragma once
#include "CollisionEngine.h"
#include "EntityState.h"
#include "MovementKnockedOver.h"

namespace Entities {
	class TinyGoomba : public Entity
	{
	public:
		TinyGoomba(const Utils::Vector2<float>& position);
		void Update(float delta) override;
		void OnReady() override;

	private:
		void OnCollision(CollisionData data);
		void OnPlayerDestroy(LPEntity _);

		void Fall(float delta);
		void Follow(float delta);
		void FallOff(float delta);

		EntityState<TinyGoomba> state;
		float time;
		float initialX;
		LPEntity targetPlayer;
		float playerHoldTime;
		bool lockHold;
		int shakeOffCount;
		LPMovement<MovementKnockedOver> fallOffMovement;

		static const float FALL_SPEED;
		static const float WAVE_FALL_OFFSET_X;
		static const float WAVE_FALL_X_DURATION;
		static const float WAVE_OFFSET_X;
		static const float WAVE_X_DURATION;
		static const float WAVE_OFFSET_Y;
		static const float WAVE_Y_DURATION;

		static const float PLAYER_FORCE_DOWN_SPEED;
		static const float MAX_HOLD_JUMP_TIME;
		static const float NUMBER_OF_SHAKE_OFFS;
	};
}


