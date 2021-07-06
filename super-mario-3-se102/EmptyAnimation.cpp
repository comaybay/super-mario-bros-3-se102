#include "EmptyAnimation.h"

const SpriteBox EmptyAnimation::EMPTY_SPRITE_BOX(
	{ 0, 0, 0, 0 }, { 0, 0 }
);

EmptyAnimation::EmptyAnimation()
	: Animation::Animation("None", -1.0f, nullptr, { EMPTY_SPRITE_BOX }) {}

void EmptyAnimation::Render(const Utils::Vector2<float>& position)
{
}

void EmptyAnimation::Update(float delta)
{
}
