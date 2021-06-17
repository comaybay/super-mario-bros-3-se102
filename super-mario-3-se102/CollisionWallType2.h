#pragma once
#include "Entity.h"
#include "Utils.h"

namespace Entities {
	class CollisionWallType2 : public Entity {
	public:
		CollisionWallType2(Utils::Vector2<float> position, Utils::Dimension dimension);
		Utils::Dimension GetDimension() override;

	private:
		Utils::Dimension dim;
	};
}