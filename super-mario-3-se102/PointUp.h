#pragma once
#include "Utils.h"
#include "Entity.h"
#include <unordered_map>

namespace Entities {

	class PointUp : public Entity
	{
	public:
		enum class Type {
			ONE_HUNDRED_POINTS,
			TWO_HUNDRED_POINTS,
			FOUR_HUNDRED_POINTS,
			EIGHT_HUNDRED_POINTS,
			ONE_THOUSAND_POINTS,
			TWO_THOUSAND_POINTS,
			FOUR_THOUSAND_POINTS,
			EIGHT_THOUSAND_POINTS,
			ONE_UP,
		};

		PointUp(const Utils::Vector2<float>& position, Type pointType);
		void Update(float delta) override;

	private:
		float time;
		static const float EXIST_DURATION;
		static const float MOVE_UP_SPEED;
		static const float FRICTION;
		static const std::unordered_map<Type, std::string> animationIdByType;
	};
}


