#include "AnimationManager.h"
#include "EmptyAnimation.h"

std::map<std::string, LPAnimation> AnimationManager::animationById;

void AnimationManager::Add(std::string id, LPAnimation anim)
{
	animationById[id] = anim;
}

LPAnimation AnimationManager::GetNew(std::string id)
{
	if (id == AnimationId::NONE)
		return new EmptyAnimation();

	if (!Utils::MapHas(id, animationById)) {
		std::string msg = "Animation \"" + id + "\" not found";
		throw std::exception(msg.c_str());
	}

	return new Animation(*animationById[id]);
}

