#pragma once
#include "Entity.h"
#include "Utils.h"
#include "CollisionEngine.h"

namespace Entities {
	class Brick : public Entity {
	public:
		Brick(LPEntity content, const Utils::Vector2<float>& position);
	private:
		void ExposeContent();
		void OnCollision(CollisionData data);
		LPEntity content;
	};
}

