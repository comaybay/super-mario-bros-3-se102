#include "MarioAnimationSet.h"

MarioAnimationSet::MarioAnimationSet(
	const std::string& idleLeft, const std::string& idleRight, const std::string& turnLeft,
	const std::string& turnRight, const std::string& walkLeft, const std::string& walkRight,
	const std::string& jumpLeft, const std::string& jumpRight, const std::string& fallLeft, const std::string& fallRight
) :
	idleLeft(idleLeft), idleRight(idleRight), turnLeft(turnLeft),
	turnRight(turnRight), walkLeft(walkLeft), walkRight(walkRight),
	jumpLeft(jumpLeft), jumpRight(jumpRight), fallLeft(fallLeft), fallRight(fallRight)
{
}
