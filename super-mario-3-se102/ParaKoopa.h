#pragma once
#include "Entity.h"
#include "Wing.h"
#include "Utils.h"
#include "EntityState.h"
#include "CollisionEngine.h"
#include "IKnockedOverable.h"

namespace Entities {
	class ParaKoopa : public Entity, public IKnockedOverable
	{
	public:
		ParaKoopa(const std::string& colorType, const Utils::Vector2<float>& position);
		void Update(float delta) override;
		void Render() override;
		void GetKnockedOver(HDirection direction) override;
		bool IsSliding();

	private:
		void OnReady() override;
		void OnCollision(CollisionData data);
		void HandleWallCollision(const CollisionData& data);
		void HandlePlayerCollision(const CollisionData& data);
		void SetWingDirection(WingDirection dir);

		void JumpAround(float delta);

		std::string colorType;
		std::string colorCode;
		Entities::Wing wing;
		EntityState<ParaKoopa> state;
		static const float JUMP_SPEED;
		static const float FALL_SPEED;
	};
}