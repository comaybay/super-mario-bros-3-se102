#include "MarioBig.h"
#include "MarioTransition.h"
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
	parentScene->AddEntity(new MarioTransition(this, PlayerPowerLevel::SMALL));
}
