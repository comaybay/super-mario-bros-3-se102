#include "EmptyAnimation.h"

EmptyAnimation::EmptyAnimation()
	: Animation::Animation("None", -1.0f, nullptr, std::vector<Utils::SpriteBox> {}) {}

void EmptyAnimation::Render(Utils::Vector2<float> position)
{
}

void EmptyAnimation::Update(float delta)
{
}
