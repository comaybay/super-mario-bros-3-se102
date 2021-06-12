#pragma once
#include "Animation.h"

class FixedAnimation : public Animation
{
public:
	FixedAnimation(std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, std::vector<Utils::SpriteBox> sequence);
	void Update(float delta) override;
};

