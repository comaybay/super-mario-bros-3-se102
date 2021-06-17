#pragma once
#include "Animation.h"
#include <map>

struct AnimationProps {
	AnimationType type;
	std::string id;
	float frameDuration;
	LPDIRECT3DTEXTURE9 texture;
	std::vector<Utils::SpriteBox> sequence;
	Hitbox hitbox;
	AnimationProps();
	AnimationProps(
		AnimationType type, std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<Utils::SpriteBox>& sequence,
		const Hitbox& hitbox
	);
};

class AnimationManager
{
public:
	static void Add(const std::string& id, const AnimationProps& anim);
	static LPAnimation GetNew(const std::string& id);
private:
	static std::map<std::string, AnimationProps> animationPropsById;
};

