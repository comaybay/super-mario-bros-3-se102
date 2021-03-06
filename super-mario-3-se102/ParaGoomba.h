#pragma once
#include "CollisionEngine.h"
#include "Wing.h"
#include "Direction.h"
#include "Color.h"
#include "IKnockedOverable.h"

namespace Entities {
	class ParaGoomba : public Entity, public IKnockedOverable
	{
	public:
		ParaGoomba(const std::string& colorType, const Utils::Vector2<float>& position);
		void PostUpdate() override;
		void OnReady() override;
		void Render() override;
		void UpdateWings(float delta);
		void FlapUpWings();
		void FlapDownWings();
		void AutoFlapWings();
		void SetWingFlapSpeed(float speed);
		void GetKnockedOver(HDirection direction) override;

	protected:
		virtual void OnCollision(CollisionData data);

		float onGround;
		std::string colorType;
		std::string colorCode;
		Entities::Wing wingLeft;
		Entities::Wing wingRight;
	};
}