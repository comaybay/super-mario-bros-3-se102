#include "EntityUtils.h"

bool EntityUtils::IsOnLeftSideOf(LPEntity entity, LPEntity other)
{
	return (other->GetPosition().x + other->GetCurrentSpriteDimension().width / 2.0f < entity->GetPosition().x + entity->GetCurrentSpriteDimension().width / 2.0f);
}
