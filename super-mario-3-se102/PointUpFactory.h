#pragma once
#include "PointUp.h"
#include "Mario.h"
#include <unordered_map>

class PointUpFactory
{
public:
	static Entities::PointUp* Create(const Utils::Vector2<float>& callerPosition);

private:
	static void OnRestartPointUp();
	static void OnEntityDestroy(LPEntity player);

	static const std::array<PointType, 9> pointUpTypes;
	static int index;
	static Entities::LPMario player;
};

