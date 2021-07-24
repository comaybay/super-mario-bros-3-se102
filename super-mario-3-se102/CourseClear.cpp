#include "CourseClear.h"
#include "PlayerVariables.h"
#include "Game.h"
#include "Scene.h"
#include "Group.h"
#include "AnimationManager.h"
#include "Scene.h"
using namespace Entities;

const float CourseClear::DURATION_TILL_EXIT = 6.0f;
const float CourseClear::DURATION_TILL_SHOW_CARD = 2.0f;

CourseClear::CourseClear(GoalRouletteCard card)
	: Entity({ 0, 0 }, "CourseClear", GridType::NONE),
	card(card),
	cardAdded(false),
	time(0),
	textAnim(AnimationManager::GetNew("HUDCC")),
	cardAnim(AnimationManager::GetNew("HUDCards"))
{

	switch (card) {
	case GoalRouletteCard::MUSHROOM:
		cardAnim->SetFrame(1);
		break;

	case GoalRouletteCard::FLOWER:
		cardAnim->SetFrame(2);
		break;

	case GoalRouletteCard::STAR:
		cardAnim->SetFrame(3);
		break;
	}
}

void CourseClear::OnReady()
{
	Entity::OnReady();
	parentScene->UnsubscribeToOutOfWorldEvent(this);

	parentScene->StopTimer();
	parentScene->GetCamera().StopFollowingEntity();
}

void CourseClear::Update(float delta)
{
	time += delta;

	if (!cardAdded && time >= DURATION_TILL_SHOW_CARD) {
		cardAdded = true;
		PlayerVariables::AddCard(card);
	}

	if (time >= DURATION_TILL_EXIT) {
		PlayerVariables::SetTimer(0);
		Game::QueueFreeAndSwitchScene(parentScene->GetPrevScenePath());
	}
}

void CourseClear::Render()
{
	if (time >= DURATION_TILL_SHOW_CARD) {
		Game::DrawHUD(textAnim->GetTexture(), textAnim->GetCurrentSpriteBox().rect, { 0 , 0 });
		Game::DrawHUD(cardAnim->GetTexture(), cardAnim->GetCurrentSpriteBox().rect, { 176 , 48 });
	}
}
