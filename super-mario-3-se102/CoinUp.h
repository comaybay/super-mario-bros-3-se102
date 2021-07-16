#pragma once
#include "PointUp.h"

namespace Entities {
	class CoinUp : public Entity
	{
	public:
		CoinUp(const Utils::Vector2<float>& position);
		CoinUp(const Utils::Vector2<float>& position, PointType pointUpType);
		void Update(float delta) override;

	private:
		PointType pointUpType;
		float time;
		static const float UP_SPEED;
		static const float FALL_SPEED;
		static const float EXIST_DURATION;
	};
}

