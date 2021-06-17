#pragma once
#include "Animation.h"

class FixedAnimation : public Animation
{
public:
	FixedAnimation(
		std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<Utils::SpriteBox>& sequence,
		const Hitbox& hitbox
	);
	void Update(float delta) override;
};

