#include "MarioSuper.h"
#include "MarioTransition.h"
#include "Constants.h"
#include "Scene.h"
using namespace Entities;

const MarioAnimationSet MarioSuper::animationSet(
	"MarioBIL", "MarioBIR", "MarioBTL", "MarioBTR", "MarioBWL", "MarioBWR", "MarioBJL", "MarioBJR", "MarioBFL", "MarioBFR"
);

MarioSuper::MarioSuper(const Utils::Vector2<float>& position, HDirection initialFacingDirection)
	: Mario(position, initialFacingDirection, animationSet, "HitboxMarioB", PlayerPowerLevel::BIG)
{
}

void MarioSuper::TakeDamage()
{
	parentScene->AddEntity(new MarioTransition(this, PlayerPowerLevel::SMALL));
}
