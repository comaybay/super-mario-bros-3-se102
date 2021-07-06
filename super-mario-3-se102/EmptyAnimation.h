#pragma once
#include "Animation.h"

class EmptyAnimation : public Animation
{
public:
	EmptyAnimation();
	void Render(const Utils::Vector2<float>& position) override;
	void Update(float delta) override;
	static const SpriteBox EMPTY_SPRITE_BOX;
};

