#pragma once
#include "Entity.h"
#include "Utils.h"

namespace Entities {
	class CollisionWallType1 : public Entity {
	public:
		CollisionWallType1(const Utils::Vector2<float>& position, const Utils::Dimension<int>& dimension);
		const Hitbox& GetHitbox() override;
		Utils::Dimension<int> GetCurrentSpriteDimension() override;

	private:
		Hitbox hitbox;
	};
}