#include "EntityUtils.h"
#include "Entity.h"
#include "EntityConstants.h"

bool EntityUtils::IsOnLeftSideOf(LPEntity entity, LPEntity other)
{
	return (other->GetPosition().x + other->GetCurrentSpriteDimension().width / 2.0f < entity->GetPosition().x + entity->GetCurrentSpriteDimension().width / 2.0f);
}

bool EntityUtils::IsAboveOf(LPEntity entity, LPEntity other)
{
	return (other->GetPosition().y + other->GetCurrentSpriteDimension().height / 2.0f < entity->GetPosition().y + entity->GetCurrentSpriteDimension().height / 2.0f);
}

float EntityUtils::Wave(float from, float to, float duration, float offset, float time)
{
	float mid = (to - from) / 2;
	float offsetTime = duration * offset;
	float waveProgress = sinf(((time + offsetTime) / duration) * (Constants::PI * 2));
	return from + mid + waveProgress * mid;
}


