#include "CourseClear.h"
#include "Game.h"
#include "Scene.h"
using namespace Entities;

const float CourseClear::TIME_TILL_EXIT = 3.0f;

CourseClear::CourseClear(GoalRouletteCard card)
	: Entity({ 0, 0 }, "CourseClear", GridType::NONE)
{
}

void CourseClear::OnReady()
{
	Entity::OnReady();
	UnsubscribeToOutOfWorldEvent();

	parentScene->GetCamera().StopFollowingEntity();
}

void CourseClear::Update(float delta)
{
	time += delta;
	if (time >= TIME_TILL_EXIT)
		Game::QueueFreeAndSwitchScene(parentScene->GetPrevScenePath());
}
