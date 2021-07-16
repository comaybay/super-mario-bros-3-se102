#pragma once
#include "Utils.h"
#include "Mario.h"
#include "CollisionEngine.h"
#include "MovementBlockHit.h"

namespace Entities {
	class ItemWoodBlock : public Entity {
	public:
		ItemWoodBlock(const std::string& contentId, const Utils::Vector2<float>& position);
		~ItemWoodBlock();
		void Update(float delta) override;
		void OnReady() override;
	private:
		void ExposeContent(LPMario player);
		void OnCollision(CollisionData data);

		void Idle(float delta);
		void Touch(float delta);
		EntityState<ItemWoodBlock> state;
		LPMovement<MovementBlockHit> blockHitMovement;
		LPEntity ghostBlock;
		std::string contentId;

		const static float BOUNCE_BACK_SPEED;
	};
}