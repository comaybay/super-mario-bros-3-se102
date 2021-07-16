#pragma once
#include "Entity.h"

namespace Entities {
	class FXSmoke : public Entity
	{
	public:
		FXSmoke(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		float time;
		static const float EXIST_DURATION;
	};
}


