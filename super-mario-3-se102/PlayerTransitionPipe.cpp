#include "PlayerTransitionPipe.h"
#include "Scene.h"
#include "Group.h"

using namespace Entities;
using namespace Utils;

const float PlayerTransitionPipe::SPEED = 30;
const float PlayerTransitionPipe::WAIT_DURATION = 0.5f;

PlayerTransitionPipe::PlayerTransitionPipe(VDirection direction)
	: Entity({ 0,0 }, "PlayerTransitions", GridType::NONE),
	dir(direction), endY(0), velY(0), time(0)
{
}

void PlayerTransitionPipe::OnReady()
{
	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		targetPlayer = parentScene->GetEntityOfGroup(Group::PLAYERS);
		targetPlayer->SetFreeze(true);
		targetPlayer->SetRenderedBeforeWorld(true);

		const Vector2<float>& playerPos = targetPlayer->GetPosition();
		targetPlayer->SetPosition({ playerPos.x + Constants::TILE_SIZE / 2.0f, playerPos.y });

		velY = dir == VDirection::DOWN ? SPEED : -SPEED;
		float targetY = targetPlayer->GetPosition().y;
		float targetDimY = targetPlayer->GetCurrentSpriteDimension().height;
		endY = dir == VDirection::DOWN ? targetY + targetDimY : targetY - targetDimY;
	}
	else
		parentScene->QueueFree(this);
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
		targetPlayer->SetRenderedBeforeWorld(false);

		parentScene->QueueFree(this);
	}
}
