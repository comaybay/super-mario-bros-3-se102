#pragma once
#include "Animation.h"

class EmptyAnimation : public Animation
{
public:
	EmptyAnimation();
	void Render(Utils::Vector2 position) override;
	void Update(float delta) override;
};

