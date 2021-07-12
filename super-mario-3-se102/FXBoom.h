#pragma once
#include "Utils.h"
#include "Entity.h"

namespace Entities {
	class FXBoom : public Entity
	{
	public:
		FXBoom(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		float time;
		static const float EXIST_DURATION;
	};
}


