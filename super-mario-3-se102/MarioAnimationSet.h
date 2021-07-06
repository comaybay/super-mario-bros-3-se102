#pragma once
#include <string>

struct MarioAnimationSet {
	std::string idleLeft;
	std::string idleRight;
	std::string turnLeft;
	std::string turnRight;
	std::string walkLeft;
	std::string walkRight;
	std::string jumpLeft;
	std::string jumpRight;
	std::string fallLeft;
	std::string fallRight;

	MarioAnimationSet(
		const std::string& idleLeft,
		const std::string& idleRight,
		const std::string& turnLeft,
		const std::string& turnRight,
		const std::string& walkLeft,
		const std::string& walkRight,
		const std::string& jumpLeft,
		const std::string& jumpRight,
		const std::string& fallLeft,
		const std::string& fallRight
	);
};