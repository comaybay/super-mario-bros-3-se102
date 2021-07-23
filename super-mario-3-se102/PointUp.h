#pragma once
#include "Entity.h"
#include "PointType.h"

#include <unordered_map>

namespace Entities {

	class PointUp : public Entity
	{
	public:
		PointUp(const Utils::Vector2<float>& position, PointType pointType);
		void Update(float delta) override;

	private:
		float time;
		static const float EXIST_DURATION;
		static const float MOVE_UP_SPEED;
		static const float FRICTION;
		static const std::unordered_map<PointType, std::string> animationIdByType;
		static const std::unordered_map<PointType, int> scoreByType;
	};
}


