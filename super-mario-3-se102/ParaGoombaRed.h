#pragma once
#include "ParaGoomba.h"
#include "EntityState.h"

namespace Entities {
	class ParaGoombaRed : public ParaGoomba
	{
	public:
		ParaGoombaRed(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void MoveAround(float delta);
		void PrepareToJump(float delta);
		void Jump(float delta);
		void Fall(float delta);

		int jumpCount;
		float time;
		EntityState<ParaGoombaRed> state;

		static const float PREPARE_JUMP_SPEED;
		static const float JUMP_SPEED;
		static const float TIME_TILL_PREPARE;
		static const int NUM_OF_PREPARE_JUMPS;
		static const float JUMP_FLAP_ANIM_SPEED;
		static const float FALL_ACCEL;
	};
}