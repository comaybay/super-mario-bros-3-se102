#pragma once
#include "Entity.h"
#include "Utils.h"
#include "CollisionEngine.h"

namespace Entities {
	class QuestionBlock : public Entity {
	public:
		QuestionBlock(LPEntity content, const Utils::Vector2<float>& position);
		void OnReady() override;
	private:
		void ExposeContent();
		void OnCollision(CollisionData data);
		LPEntity content;
	};
}