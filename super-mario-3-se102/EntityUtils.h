#pragma once
#include "Entity.h"

namespace EntityUtils
{
	/// <summary>
	/// Returns true if other entity is on left side of entity
	/// </summary>
	bool IsOnLeftSideOf(LPEntity entity, LPEntity other);

	/// <summary>
	/// Returns true if other entity is above entity
	/// </summary>
	bool IsOnAboveOf(LPEntity entity, LPEntity other);

};

