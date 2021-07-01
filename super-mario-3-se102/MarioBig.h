#pragma once
#include "Mario.h"
#include "Utils.h"

namespace Entities {
	class MarioBig : public Mario {
	public:
		MarioBig(const Utils::Vector2<float>& position);
		virtual void TakeDamage();

	private:
		static const Mario::AnimationSet animationSet;
	};
}