#include "WMHelpBubble.h"
#include "Group.h"
using namespace Entities;

const float WMHelpBubble::ANIM_DURATION = 1.1f;
const float WMHelpBubble::SHOW_DURATION = 0.9f;

WMHelpBubble::WMHelpBubble(const Utils::Vector2<float>& position)
	: Entity::Entity(position, "WMHelpBubble", HitboxId::NONE, Group::EFFECTS, GridType::STATIC_ENTITIES),
	time(0)
{
}

void WMHelpBubble::Update(float delta)
{
	time += delta;
	if (time >= ANIM_DURATION)
		time -= ANIM_DURATION;

	SetAnimation((time <= SHOW_DURATION) ? "WMHelpBubble" : AnimationId::NONE);
}

