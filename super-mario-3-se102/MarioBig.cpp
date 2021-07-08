#include "MarioBig.h"
#include "MarioSmall.h"
#include "Constants.h"
#include "Scene.h"
using namespace Entities;

const MarioAnimationSet MarioBig::animationSet(
	"MarioBIL", "MarioBIR", "MarioBTL", "MarioBTR", "MarioBWL", "MarioBWR", "MarioBJL", "MarioBJR", "MarioBFL", "MarioBFR"
);

MarioBig::MarioBig(const Utils::Vector2<float>& position, HDirection initialFacingDirection)
	: Mario(position, initialFacingDirection, animationSet, "HitboxMarioB", PlayerPowerLevel::BIG)
{
}

void MarioBig::TakeDamage()
{
	parentScene->QueueFree(this);
	Utils::Vector2<float> smallMarioPos = { position.x, position.y + Constants::TILE_SIZE };
	parentScene->AddEntity(new MarioSmall(smallMarioPos, GetFacingDirection()));
}
