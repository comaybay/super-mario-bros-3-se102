#pragma once
#include "Entity.h"
#include "Utils.h"
#include "CollisionEngine.h"
#include "MovementKnockOver.h"
#include "EntityState.h"

namespace Entities {
	class Goomba : public Entity
	{
	public:
		Goomba(std::string colorType, Utils::Vector2<float> position);
		void Update(float delta) override;
		void KnockOver(HDirection direction);

	private:
		virtual void OnReady() override;
		void OnCollision(CollisionData data);

		void MoveAround(float delta);
		void StompedOn(float delta);
		void KnockedOver(float delta);

		std::string colorCode;
		float time;
		EventHandler<CollisionData> onCollisionHandler;
		EntityState<Goomba> state;
		LPMovement<MovementKnockOver> knockOverMovement;

	public:
		static const float WALK_SPEED;
	};
}