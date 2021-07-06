#pragma once
#include "Entity.h"
#include "GoalRouletteCard.h"

namespace Entities {
	class CourseClear : public Entity
	{
	public:
		CourseClear(GoalRouletteCard card);
		void OnReady() override;
		void Update(float delta) override;

	private:
		float time;
		GoalRouletteCard card;

		static const float TIME_TILL_EXIT;
	};
}