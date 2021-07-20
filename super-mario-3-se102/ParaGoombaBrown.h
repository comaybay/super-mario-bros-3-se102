#pragma once
#include "ParaGoomba.h"
#include "EntityState.h"

namespace Entities {
	class ParaGoombaBrown : public ParaGoomba
	{
	public:
		ParaGoombaBrown(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void OnPlayerDestroy(LPEntity _);
		LPEntity TryFindPlayer();

		void MoveAround(float delta);
		void PrepareToFly(float delta);
		void FlyUp(float delta);
		void Turn(float delta);
		void Fall(float delta);

		float GetVelocityXTowardPlayer();
		float time;
		EntityState<ParaGoombaBrown> state;
		LPEntity targetPlayer;
		float flyBasePosY;
		float xStartTurn;
		int turnDir;
		int dropCount;

		static const float WAIT_TIME;
		static const float FALL_ACCEL;
		static const float TURN_ACCEL;
		static const float MAX_TURN_SPEED;
		static const float FLY_SPEED;
		static const float FLY_OFFSET_Y;
		static const float FLY_WAVE_Y_DUARTION;
		static const float DISTANCE_ABOVE_PLAYER;
		static const float DISTANCE_BEFORE_TURN;
		static const float NUMBER_OF_DROPS;
		static const float DROP_TINY_GOOMBA_RATE;
	};
}
