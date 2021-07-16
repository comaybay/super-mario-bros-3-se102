#include "AnimationManager.h"
#include "EmptyAnimation.h"
#include "FixedAnimation.h"
#include "Contains.h"

std::unordered_map<std::string, AnimationProps> AnimationManager::animationPropsById;

AnimationProps::AnimationProps() :
	type(AnimationType::NORMAL), id(""), frameDuration(0), texture(nullptr), sequence({})
{
}

AnimationProps::AnimationProps(AnimationType type, std::string id, float frameDuration, LPDIRECT3DTEXTURE9 texture, const std::vector<SpriteBox>& sequence)
	: type(type), id(id), frameDuration(frameDuration), texture(texture), sequence(sequence)
{
}

void AnimationManager::Add(const std::string& id, const AnimationProps& animProps)
{
	animationPropsById[id] = animProps;
}

LPAnimation AnimationManager::GetNew(const std::string& id)
{
	if (id == AnimationId::NONE)
		return new EmptyAnimation();

	if (!Utils::Contains(id, animationPropsById)) {
		std::string msg = "Animation \"" + id + "\" not found";
		throw std::exception(msg.c_str());
	}

	const AnimationProps& props = animationPropsById[id];
	switch (props.type) {
	case AnimationType::NORMAL:
		return new Animation(props.id, props.frameDuration, props.texture, props.sequence);
	case AnimationType::FIXED:
		return new FixedAnimation(props.id, props.frameDuration, props.texture, props.sequence);
	default:
		throw std::exception("GetNew Failed: Not implemented animation type.");
	}
}

