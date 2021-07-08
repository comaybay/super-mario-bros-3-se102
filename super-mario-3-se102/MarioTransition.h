#pragma once
#include "Mario.h" 
#include "PlayerPowerLevel.h"
#include "Direction.h" 

namespace Entities {
	/// <summary>
	/// <para> Use this when Mario need to change his power level </para>
	/// <para> MarioTransition will queue free given Mario, play an animation.
	/// When finished, it will add new Mario (with given power level) to active Scene </para>
	/// </summary>
	class MarioTransition : public Entity
	{
	public:
		/// <summary>
		/// Notes that param "to" is not the power level of Mario after transition, depends on current mario power level,
		/// the resulting power level might be different
		/// </summary>
		MarioTransition(LPMario player, PlayerPowerLevel to);
		void OnReady() override;
		void Update(float delta) override;

	private:
		void SmallToBigPart1(float delta);
		void SmallToBigPart2(float delta);
		void Smoke(float delta);

		void CreateMario();

		float time;
		LPMario player;
		bool isDowngradeTransiton;
		std::string animDirCode;
		PlayerPowerLevel resultPowerLevel;
		EntityState<MarioTransition> state;

		static const float STB_PART1_DURATION;
		static const float STB_PART2_DURATION;
		static const float SMOKE_DURATION;
	};
}