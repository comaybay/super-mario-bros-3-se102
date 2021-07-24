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
		void Render() override;

	private:
		float time;
		bool cardAdded;
		GoalRouletteCard card;
		LPAnimation textAnim;
		LPAnimation cardAnim;

		static const float DURATION_TILL_EXIT;
		static const float DURATION_TILL_SHOW_CARD;
	};
}