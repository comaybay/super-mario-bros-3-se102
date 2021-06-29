#pragma once
#include "Entity.h"
#include "Utils.h"
#include "CollisionEngine.h"

namespace Entities {
	class Coin : public Entity {
	public:
		Coin(const Utils::Vector2<float>& position);
		void OnCollision(CollisionData data);
		void OnReady() override;
	};
}

