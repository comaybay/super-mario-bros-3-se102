#include "EntityUtils.h"

bool EntityUtils::IsOnLeftSideOf(LPEntity entity, LPEntity other)
{
	return (other->GetPosition().x + other->GetCurrentSpriteDimension().width / 2.0f < entity->GetPosition().x + entity->GetCurrentSpriteDimension().width / 2.0f);
}

bool EntityUtils::IsAboveOf(LPEntity entity, LPEntity other)
{
	return (other->GetPosition().y + other->GetCurrentSpriteDimension().height / 2.0f < entity->GetPosition().y + entity->GetCurrentSpriteDimension().height / 2.0f);
}

