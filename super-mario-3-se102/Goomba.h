#pragma once
#include "Entity.h"
#include "Utils.h"
#include "CollisionEngine.h"
#include "EntityState.h"

namespace Entities {
	class Goomba : public Entity
	{
	public:
		Goomba(std::string color, Utils::Vector2<float> position);
		void Update(float delta) override;

	private:
		void MoveAround(float delta);
		void Die(float delta);

		std::string colorCode;
		float time;
		Utils::Vector2<float> speed;
		EventHandler<CollisionData> onCollisionHandler;
		EntityState<Goomba> state;
		virtual void OnReady() override;
		void OnCollision(CollisionData data);
	};
}