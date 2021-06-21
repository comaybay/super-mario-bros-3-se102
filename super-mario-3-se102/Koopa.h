#pragma once
#include "Entity.h"
#include "Utils.h"
#include "EntityState.h"
#include "CollisionEngine.h"

namespace Entities {
	class Koopa : public Entity
	{
	public:
		Koopa(const std::string& colorType, const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void OnReady() override;
		void OnCollision(CollisionData data);
		void HandleWallCollision(const CollisionData& data);
		void HandlePlayerCollision(const CollisionData& data);

		void SwitchState(EntityState<Koopa>::Handler handler);
		void MoveAround(float delta);
		void ShellIdle(float delta);
		void ShellSlide(float delta);

		std::string colorCode;
		EntityState<Koopa> state;
		static const float WALK_SPEED;
		static const float SHELL_SLIDE_SPEED;
		static const float FRICTION;
	};
}