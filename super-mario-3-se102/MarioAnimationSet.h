#pragma once
#include <string>

struct MarioAnimationSet {
	std::string idleLeft;
	std::string idleRight;
	std::string turnLeft;
	std::string turnRight;
	std::string walkLeft;
	std::string walkRight;
	std::string runLeft;
	std::string runRight;
	std::string jumpLeft;
	std::string jumpRight;
	std::string runJumpLeft;
	std::string runJumpRight;
	std::string fallLeft;
	std::string fallRight;
	std::string runFallLeft;
	std::string runFallRight;
	std::string duckLeft;
	std::string duckRight;
	std::string idleHoldLeft;
	std::string idleHoldRight;
	std::string holdLeft;
	std::string holdRight;
	std::string goInPipe;

	MarioAnimationSet(
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
		const std::string& runJumpLeft,
		const std::string& runJumpRight,
		const std::string& fallLeft,
		const std::string& fallRight,
		const std::string& runFallLeft,
		const std::string& runFallRight,
		const std::string& duckLeft,
		const std::string& duckRight,
		const std::string& idleHoldLeft,
		const std::string& idleHoldRight,
		const std::string& holdLeft,
		const std::string& holdRight,
		const std::string& goInPipe
	);
};