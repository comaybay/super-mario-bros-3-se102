#pragma once
#include "CollisionEngine.h"

namespace Entities {
	class VenusFireBall : public Entity
	{
	public:
		VenusFireBall(const Utils::Vector2<float>& position, float rad);
		void OnReady() override;

	private:
		void OnCollision(CollisionData data);

		static const float SPEED;
	};
}


