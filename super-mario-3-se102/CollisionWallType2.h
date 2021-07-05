#pragma once
#include "Entity.h"
#include "Utils.h"

namespace Entities {
	class CollisionWallType2 : public Entity {
	public:
		CollisionWallType2(Utils::Vector2<float> position, Utils::Dimension<int> dimension);
		LPConstHitbox GetHitbox() override;
		Utils::Dimension<int> GetCurrentSpriteDimension() override;

	private:
		Hitbox hitbox;
	};
}

