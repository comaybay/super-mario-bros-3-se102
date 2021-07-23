#include "PlayerTransitionSuperNote.h"
#include "Scene.h"
#include "Group.h"

using namespace Entities;
using namespace Utils;

const float PlayerTransitionSuperNote::SPEED = 400;

PlayerTransitionSuperNote::PlayerTransitionSuperNote()
	: Entity({ 0,0 }, "PlayerTransitions", GridType::NONE)
{
}

void PlayerTransitionSuperNote::OnReady() {
	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity targetPlayer = parentScene->GetEntityOfGroup(Group::PLAYERS);
		targetPlayer->SetVelocity({ 0, -SPEED });
	}

	parentScene->QueueFree(this);
}
