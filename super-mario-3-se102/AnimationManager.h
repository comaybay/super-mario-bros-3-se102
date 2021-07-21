#pragma once
#include "Animation.h"
#include <unordered_map>

struct AnimationProps {
	AnimationType type;
	std::string id;
	float frameDuration;
	LPDIRECT3DTEXTURE9 texture;
	std::vector<SpriteBox> sequence;
	bool loopAnim;
	AnimationProps();
	AnimationProps(AnimationType type, std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<SpriteBox>& sequence, bool loopAnim);
};

class AnimationManager
{
public:
	static void Add(const std::string& id, const AnimationProps& anim);
	static LPAnimation GetNew(const std::string& id);
private:
	static std::unordered_map<std::string, AnimationProps> animationPropsById;
};

