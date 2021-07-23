#include "PlayerTransitionSuperNote.h"
#include "Scene.h"
#include "Group.h"

using namespace Entities;
using namespace Utils;

const float PlayerTransitionSuperNote::SPEED = 400;

PlayerTransitionSuperNote::PlayerTransitionSuperNote(LPEntity entity)
	: Entity({ 0,0 }, "PlayerTransitions", GridType::NONE)
{
	entity->SetVelocity({ 0, -SPEED });
}

void PlayerTransitionSuperNote::OnReady() {
	parentScene->QueueFree(this);
}
