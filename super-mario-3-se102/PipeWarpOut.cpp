#include "PipeWarpOut.h"
#include "PlayerTransitionPipe.h"
#include "Group.h"
#include "Game.h"
#include "PlayerVariables.h"
#include "Mario.h"

using namespace Entities;
using namespace Utils;

PipeWarpOut::PipeWarpOut(const std::string& scenePath, VDirection inDirection, const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxPipeWarp", { "PipeWarps" , Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 }, GridType::STATIC_ENTITIES),
	dir(inDirection), scenePath(scenePath)
{
}

void PipeWarpOut::OnReady()
{
	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS) && PlayerVariables::IsInPipe() && PlayerVariables::GetPreviousScenePath() == scenePath) {
		LPMario player = static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS));

		Vector2<float> newPos = { position.x + Constants::TILE_SIZE / 2.0f, position.y };

		if (player->GetPowerLevel() != PlayerPowerLevel::SMALL)
			newPos.y += (dir == VDirection::DOWN) ? -Constants::TILE_SIZE : Constants::TILE_SIZE;

		player->SetPosition(newPos);

		parentScene->AddEntity(new PlayerTransitionPipe(player, dir));
		PlayerVariables::SetIsInPipe(false);
	}
}
