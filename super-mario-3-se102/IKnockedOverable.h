#pragma once
#include "Direction.h"

class IKnockedOverable {
public:
	virtual void GetKnockedOver(HDirection direction) = 0;
};