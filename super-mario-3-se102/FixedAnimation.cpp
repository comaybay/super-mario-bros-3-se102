#include "FixedAnimation.h"
#include "Utils.h"

FixedAnimation::FixedAnimation(std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, std::vector<Utils::SpriteBox> sequence)
	: Animation(id, frameDuration, texture, sequence)
{}

void FixedAnimation::Update(float _)
{
	currentFrame = (int)((GetTickCount64() / 1000.0f) / frameDuration) % sequence.size();
}
