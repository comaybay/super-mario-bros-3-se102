#include "PlayerTransitionPipe.h"
#include "Scene.h"
#include "Group.h"

using namespace Entities;
using namespace Utils;

const float PlayerTransitionPipe::SPEED = 30;
const float PlayerTransitionPipe::WAIT_DURATION = 0.25f;

PlayerTransitionPipe::PlayerTransitionPipe(LPMario player, VDirection direction, const Utils::Vector2<float>& offset)
	: Entity({ 0,0 }, { "PlayerTransitions", Group::NOT_AFFECTED_BY_TRANSITION_PAUSE }, GridType::NONE),
	targetPlayer(player), dir(direction), endY(0), velY(0), time(0), offset(offset)
{
}

void PlayerTransitionPipe::OnReady()
{
	parentScene->TransitionPause(true);
	targetPlayer->ShowGoInPipeAnimation();
	targetPlayer->SetFreeze(true);
	targetPlayer->SetRenderedBeforeForeground(true);
	targetPlayer->SetPosition(targetPlayer->GetPosition() + offset);

	velY = dir == VDirection::DOWN ? SPEED : -SPEED;
	float targetY = targetPlayer->GetPosition().y;
	float targetDimY = targetPlayer->GetCurrentSpriteDimension().height;
	endY = dir == VDirection::DOWN ? targetY + targetDimY : targetY - targetDimY;

}

void PlayerTransitionPipe::Update(float delta)
{
	time += delta;
	if (time < WAIT_DURATION)
		return;

	const Vector2<float>& playerPos = targetPlayer->GetPosition();
	Vector2<float> newPos = { playerPos.x, playerPos.y + velY * delta };
	targetPlayer->SetPosition(newPos);

	if (dir == VDirection::DOWN && newPos.y >= endY ||
		dir == VDirection::UP && newPos.y <= endY)
	{
		targetPlayer->SetPosition({ newPos.x, endY });
		targetPlayer->SetFreeze(false);
		targetPlayer->SetRenderedBeforeForeground(false);

		parentScene->TransitionPause(false);
		transitionCompleteEvent.Notify();
		parentScene->QueueFree(this);
	}
}

Event<>& PlayerTransitionPipe::GetTransitionCompleteEvent()
{
	return transitionCompleteEvent;
}
