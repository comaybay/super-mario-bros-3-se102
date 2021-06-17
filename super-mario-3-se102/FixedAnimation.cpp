#include "FixedAnimation.h"
#include "Utils.h"

FixedAnimation::FixedAnimation(
	std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<Utils::SpriteBox>& sequence,
	const Hitbox& hitbox
)
	: Animation(id, frameDuration, texture, sequence, hitbox)
{}

void FixedAnimation::Update(float _)
{
	currentFrame = (ULONGLONG)((GetTickCount64() / 1000.0f) / frameDuration) % sequence.size();
}
