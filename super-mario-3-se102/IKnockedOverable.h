#pragma once
#include "Direction.h"

/// <summary>
/// Inteface for entities that can be knocked over (by Koopa's shell, etc.)
/// </summary>
class IKnockedOverable {
public:
	virtual void GetKnockedOver(HDirection direction) = 0;

	//virtual void GetKnockedOverByPlayer(HDirection direction) = 0;
};