#include "MarioBig.h"
#include "MarioSmall.h"
#include "Game.h"
#include "Scene.h"
using namespace Entities;

const Mario::AnimationSet MarioBig::animationSet = Mario::AnimationSet(
	"MarioBIL", "MarioBIR", "MarioBTL", "MarioBTR", "MarioBML", "MarioBMR", "MarioBJL", "MarioBJR"
);

MarioBig::MarioBig(const Utils::Vector2<float>& position)
	: Mario(position, animationSet, PlayerPowerLevel::BIG)
{
}

void MarioBig::TakeDamage()
{
	parentScene->QueueFree(this);
	parentScene->AddEntity(new MarioSmall(position + Utils::Vector2<float>(0, Game::TILE_SIZE)));
}
