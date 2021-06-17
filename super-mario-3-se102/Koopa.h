#pragma once
#include "Entity.h"
#include "Utils.h"
#include "EntityState.h"
#include "CollisionEngine.h"

namespace Entities {
	class Koopa : public Entity
	{
	public:
		Koopa(const std::string& colorCode, const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void OnReady() override;
		void OnCollision(CollisionData data);

		void MoveAround(float delta);

		std::string colorCode;
		EntityState<Koopa> state;
		static const Utils::Vector2<float> SPEED;
	};
}