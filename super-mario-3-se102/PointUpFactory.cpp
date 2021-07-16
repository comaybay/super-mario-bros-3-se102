#include "PointUpFactory.h"
#include "Constants.h"
#include "Game.h"
#include "Utils.h"
#include "Event.h"
#include "Group.h"
#include <array>
using namespace Utils;
using namespace Entities;

int PointUpFactory::index = -1;
//TODO: use an abstraction of a player instead of Mario.
LPMario PointUpFactory::player;

const std::array<PointUp::Type, 9> PointUpFactory::pointUpTypes{
	PointUp::Type::ONE_HUNDRED_POINTS,
	PointUp::Type::TWO_HUNDRED_POINTS,
	PointUp::Type::FOUR_HUNDRED_POINTS,
	PointUp::Type::EIGHT_HUNDRED_POINTS,
	PointUp::Type::ONE_THOUSAND_POINTS,
	PointUp::Type::TWO_THOUSAND_POINTS,
	PointUp::Type::FOUR_THOUSAND_POINTS,
	PointUp::Type::EIGHT_THOUSAND_POINTS,
	PointUp::Type::ONE_UP
};

//TODO: create a better implementation
PointUp* PointUpFactory::Create(const Vector2<float>& callerPosition)
{
	Vector2<float> pos = { callerPosition.x, callerPosition.y - Constants::TILE_SIZE };

	if (player == nullptr) {
		LPScene scene = Game::GetActiveScene();

		if (scene->IsEntityGroupEmpty(Group::PLAYERS))
			return new PointUp(pos, PointUp::Type::ONE_HUNDRED_POINTS);

		player = static_cast<LPMario>(scene->GetEntityOfGroup(Group::PLAYERS));
		player->GetRestartPointUpEvent().Subscribe(&OnRestartPointUp);
		player->GetDestroyEvent().Subscribe(&OnEntityDestroy);
	}

	index = min(index + 1, (int)pointUpTypes.size() - 1);
	return new PointUp(pos, pointUpTypes[index]);
}

void PointUpFactory::OnRestartPointUp() {
	index = -1;
}
void PointUpFactory::OnEntityDestroy(LPEntity _) {

	PointUpFactory::player = nullptr;
	index = -1;
}

