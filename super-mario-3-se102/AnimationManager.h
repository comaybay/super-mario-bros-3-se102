#pragma once
#include "Animation.h"
#include <map>

class AnimationManager
{
public:
	static void Add(std::string id, LPAnimation anim);
	static LPAnimation GetNew(std::string id);
private:
	static std::map<std::string, LPAnimation> animationById;
};

