#pragma once
#include "Mario.h"
#include "Utils.h"

namespace Entities {
	class MarioSuper : public Mario {
	public:
		MarioSuper(const Utils::Vector2<float>& position, HDirection direction = HDirection::RIGHT);
		virtual void TakeDamage();

	private:
		static const MarioAnimationSet animationSet;
	};
}