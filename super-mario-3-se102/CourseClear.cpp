#include "CourseClear.h"
#include "PlayerVariables.h"
#include "Game.h"
#include "Scene.h"
#include "Mario.h"
#include "Group.h"
using namespace Entities;

const float CourseClear::PREPARE_EXIT_DURATION = 3.0f;

CourseClear::CourseClear(GoalRouletteCard card)
	: Entity({ 0, 0 }, "CourseClear", GridType::NONE)
{
}

void CourseClear::OnReady()
{
	Entity::OnReady();
	parentScene->UnsubscribeToOutOfWorldEvent(this);

	parentScene->GetCamera().StopFollowingEntity();

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPMario player = static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS));
		PlayerVariables::SetPlayerPowerLevel(player->GetPowerLevel());
	}
}

void CourseClear::Update(float delta)
{
	time += delta;
	if (time >= PREPARE_EXIT_DURATION) {
		Game::QueueFreeAndSwitchScene(parentScene->GetPrevScenePath());
	}
}
