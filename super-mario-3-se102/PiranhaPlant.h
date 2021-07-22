#pragma once
#include "EntityState.h"
#include "CollisionEngine.h"
#include "IKnockedOverable.h"

namespace Entities {
	class PiranhaPlant : public Entity, public IKnockedOverable
	{
	public:
		PiranhaPlant(const std::string& colorType, const Utils::Vector2<float>& position);
		void Update(float delta) override;

		void GetKnockedOver(HDirection direction) override;

	private:
		void OnReady() override;
		void OnPlayerDestroy(LPEntity entity);
		void OnCollision(CollisionData data);
		LPEntity TryFindPlayer();

		void PrepareCheckDistance(float delta);
		void CheckDistance(float delta);
		void MoveUp(float delta);
		void PrepareMoveDown(float delta);
		void MoveDown(float delta);

		LPEntity targetPlayer;
		EntityState<PiranhaPlant> state;
		const float stopYMoveUp;
		const float stopYMoveDown;
		float time;

		static const float WAIT_DURATION;
		static const float MOVE_SPEED;
		static const float SAFE_DISTANCE;
	};
}