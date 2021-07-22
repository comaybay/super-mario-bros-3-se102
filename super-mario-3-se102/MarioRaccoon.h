#pragma once
#include "Mario.h"

namespace Entities {
	class MarioRaccoon : public Mario
	{
	public:
		MarioRaccoon(const Utils::Vector2<float>& position, HDirection direction = HDirection::RIGHT);
		void Update(float delta) override;
		void TakeDamage() override;

	private:
		void WaitFlap(float delta);
		void FlyCountDown(float delta);
		void FlyFlap(float delta);

		void WaitFallFlap(float delta);
		void FallFlap(float delta);

		void WaitSpin(float delta);
		void Spin(float delta);

		void UpdateAnimation() override;
		void HandleJumpStateMovement(float delta) override;
		void HandleFallStateMovement(float delta) override;

		EntityState<MarioRaccoon> flapState;
		EntityState<MarioRaccoon> spinState;
		float time;
		float powerMeterTime;
		float jumpCount;
		float isUsingPowerMeter;
		bool isOnFlyFlapAnim;
		bool isOnFallFlapAnim;

		static const MarioAnimationSet animationSet;
		static const float FLY_FLAP_SPEED;
		static const float FALL_FLAP_SPEED;
		static const float FLAP_DURATION;
		static const float POWER_METER_DURATION;
	};
}