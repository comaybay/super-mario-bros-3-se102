#include "MarioAnimationSet.h"

MarioAnimationSet::MarioAnimationSet(
	const std::string& idleLeft,
	const std::string& idleRight,
	const std::string& turnLeft,
	const std::string& turnRight,
	const std::string& walkLeft,
	const std::string& walkRight,
	const std::string& runLeft,
	const std::string& runRight,
	const std::string& jumpLeft,
	const std::string& jumpRight,
	const std::string& fallLeft,
	const std::string& fallRight,
	const std::string& duckLeft,
	const std::string& duckRight,
	const std::string& idleHoldLeft,
	const std::string& idleHoldRight,
	const std::string& holdLeft,
	const std::string& holdRight
) :
	idleLeft(idleLeft),
	idleRight(idleRight),
	turnLeft(turnLeft),
	turnRight(turnRight),
	walkLeft(walkLeft),
	walkRight(walkRight),
	runLeft(runLeft),
	runRight(runRight),
	jumpLeft(jumpLeft),
	jumpRight(jumpRight),
	fallLeft(fallLeft),
	fallRight(fallRight),
	duckLeft(duckLeft),
	duckRight(duckRight),
	idleHoldLeft(idleHoldLeft),
	idleHoldRight(idleHoldRight),
	holdLeft(holdLeft),
	holdRight(holdRight)
{
}
