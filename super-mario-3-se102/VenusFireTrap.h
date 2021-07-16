#pragma once
#include "EntityState.h"
#include "CollisionEngine.h"

namespace Entities {
	class VenusFireTrap : public Entity
	{
	public:
		VenusFireTrap(const std::string& colorType, const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		void OnReady() override;
		void OnPlayerDestroy(LPEntity entity);
		void OnCollision(CollisionData data);
		void UpdateHeadAnimation();
		LPEntity TryFindPlayer();

		void PrepareCheckDistance(float delta);
		void CheckDistance(float delta);
		void MoveUp(float delta);
		void WaitAndShoot(float delta);
		void PrepareMoveDown(float delta);
		void MoveDown(float delta);

		std::string colorCode;
		LPEntity targetPlayer;
		EntityState<VenusFireTrap> state;
		const float stopYMoveUp;
		const float stopYMoveDown;
		float time;

		static const float OFFSET_ANGLE_LOW;
		static const float OFFSET_ANGLE_HIGH;
		static const float MAX_LOW_ANGLE_RIGHT;
		static const float MAX_LOW_ANGLE_LEFT;
		static const float WAIT_DURATION;
		static const float MOVE_SPEED;
		static const float SAFE_DISTANCE;
	};
}