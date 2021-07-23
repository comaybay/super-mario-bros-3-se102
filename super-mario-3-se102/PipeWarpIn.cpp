#include "PipeWarpIn.h"
#include "PlayerTransitionPipe.h"
#include "Group.h"
#include "Game.h"
#include "PlayerVariables.h"
#include "Mario.h"

using namespace Entities;
using namespace Utils;

PipeWarpIn::PipeWarpIn(const std::string& scenePath, VDirection inDirection, const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxPipeWarp", { "PipeWarps" , Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 }, GridType::STATIC_ENTITIES),
	dir(inDirection), scenePath(scenePath)
{
}

void PipeWarpIn::OnReady()
{
	CollisionEngine::Subscribe(this, &PipeWarpIn::OnCollision, { Group::PLAYERS });
}

void PipeWarpIn::OnCollision(CollisionData data)
{
	LPEntity player = data.who;

	bool condition = (
		data.edge.y == (dir == VDirection::DOWN ? 1 : -1) &&
		player->GetPosition().x > position.x + 1 &&	// offset of 1 because the body of pipe is smaller than top
		player->GetPosition().x + Constants::TILE_SIZE < position.x + Constants::TILE_SIZE * 2 - 1 &&
		Game::IsKeyDown((dir == VDirection::DOWN) ? DIK_DOWN : DIK_UP)
		);

	if (condition) {
		LPPlayerTransitionPipe transition = new PlayerTransitionPipe(static_cast<LPMario>(player), dir);
		transition->GetTransitionCompleteEvent().Subscribe(this, &PipeWarpIn::OnTransitionComplete);
		parentScene->AddEntity(transition);
		CollisionEngine::Unsubscribe(this, &PipeWarpIn::OnCollision);
	}
}

void PipeWarpIn::OnTransitionComplete()
{
	PlayerVariables::SetIsInPipe(true);
	Game::QueueFreeAndSwitchScene(scenePath);
}
