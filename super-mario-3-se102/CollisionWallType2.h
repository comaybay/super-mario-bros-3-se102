#pragma once
#include "Entity.h"
#include "Utils.h"

namespace Entities {
	class CollisionWallType2 : public Entity {
	public:
		CollisionWallType2(Utils::Vector2<float> position, Utils::Dimension<float> dimension);
		LPConstHitbox GetHitbox() override;
		Utils::Dimension<float> GetCurrentSpriteDimension() override;

	private:
		Hitbox hitbox;
	};
}

