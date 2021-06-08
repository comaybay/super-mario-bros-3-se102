#pragma once
#include "EntityManager.h"
#include "CollisionEngine.h"

class CollisionHandling
{
public:
	static void Slide(LPEntity entity, const CollisionData& data);
};

