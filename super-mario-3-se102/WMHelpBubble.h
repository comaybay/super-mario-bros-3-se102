#pragma once
#include "Entity.h"

namespace Entities {
	class WMHelpBubble : public Entity {
	public:
		WMHelpBubble(const Utils::Vector2<float>& position);
		void Update(float delta) override;

	private:
		float time;
		static const float ANIM_DURATION;
		static const float SHOW_DURATION;
	};
}