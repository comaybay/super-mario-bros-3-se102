#pragma once
#include "Utils.h"
#include "Mario.h"
#include "CollisionEngine.h"
#include "IKnockedOverable.h"

namespace Entities {
	class QuestionBlock : public Entity, public IKnockedOverable {
	public:
		QuestionBlock(const Utils::Vector2<float>& position);
		QuestionBlock(const std::string& contentId, const Utils::Vector2<float>& position);
		void OnReady() override;

		void GetKnockedOver(HDirection direction) override;
	private:
		void ExposeContent(LPMario player);
		void OnCollision(CollisionData data);

		std::string contentId;
	};
}