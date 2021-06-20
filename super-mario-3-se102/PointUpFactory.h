#pragma once
#include "PointUp.h"
#include "Mario.h"
#include <unordered_map>

class PointUpFactory
{
public:
	static Entities::PointUp* Create(const Utils::Vector2<float>& callerPosition);

private:
	static Entities::PointUp* CreatePointByPointLevel(const Utils::Vector2<float>& position);
	static void OnRestartPointUp(LPEntity player);
	static void OnEntityDestroy(LPEntity player);

	static std::array<Entities::PointUp::Type, 9> pointUpTypes;
	static int index;
	static Entities::Mario* player;
};

