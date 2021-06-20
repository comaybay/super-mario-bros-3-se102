#include "PointUpFactory.h"
#include "Game.h"
#include "Utils.h"
#include "Event.h"
#include "Groups.h"
#include <array>
using namespace Utils;
using namespace Entities;

int PointUpFactory::index = -1;
//TODO: use an abstraction of a player instead of Mario.
Mario* PointUpFactory::player;

std::array<PointUp::Type, 9> PointUpFactory::pointUpTypes{
	PointUp::Type::ONE_HUNDRED_POINTS,
	PointUp::Type::TWO_HUNDRED_POINTS,
	PointUp::Type::FOUR_HUNDRED_POINTS,
	PointUp::Type::EIGHT_HUNDRED_POINTS,
	PointUp::Type::ONE_THOUSAND_POINTS,
	PointUp::Type::TWO_HUNDRED_POINTS,
	PointUp::Type::FOUR_THOUSAND_POINTS,
	PointUp::Type::EIGHT_THOUSAND_POINTS,
	PointUp::Type::ONE_UP
};

PointUp* PointUpFactory::Create(const Vector2<float>& callerPosition)
{
	if (player == nullptr) {
		player = static_cast<Mario*>(Game::GetActiveScene()->GetEntitiesByGroup(Groups::PLAYER).front());
		player->GetRestartPointUpEvent().Subscribe(&OnRestartPointUp);
		player->GetDestroyEvent().Subscribe(&OnEntityDestroy);
	}

	index = min(index + 1, pointUpTypes.size() - 1);
	Vector2<float> pos(callerPosition.x, callerPosition.y - Game::TILE_SIZE);
	return new PointUp(pos, pointUpTypes[index]);
}

void PointUpFactory::OnRestartPointUp(LPEntity player) {
	index = -1;
}

void PointUpFactory::OnEntityDestroy(LPEntity player) {
	player = nullptr;
}

