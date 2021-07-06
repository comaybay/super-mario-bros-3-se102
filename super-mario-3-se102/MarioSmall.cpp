#include "MarioSmall.h"
#include "EntityConstants.h"
#include "Game.h"
using namespace Entities;

const MarioAnimationSet MarioSmall::animationSet(
	"MarioSIL", "MarioSIR", "MarioSTL", "MarioSTR", "MarioSWL", "MarioSWR", "MarioSJL", "MarioSJR", "MarioSJL", "MarioSJR"
);

MarioSmall::MarioSmall(Utils::Vector2<float> position)
	: Mario(position, animationSet, PlayerPowerLevel::SMALL),
	smallMarioState(this),
	died(false)
{
}

void MarioSmall::TakeDamage()
{
	if (died)
		return;

	parentScene->PlayerDeath();
	SetAnimation("MarioDeath");
	UnsubscribeToCollisionEngine();
	time = 0;
	velocity = { 0, 0 };

	died = true;
	smallMarioState.SetHandler(&MarioSmall::Die);
}

void MarioSmall::Update(float delta)
{
	if (died) {
		Entity::Update(delta);
		smallMarioState.Handle(delta);
	}
	else
		Mario::Update(delta);
}

void MarioSmall::OnCollision(CollisionData data)
{
	if (!died)
		Mario::OnCollision(data);
}


void MarioSmall::OnOutOfWorld()
{
	if (smallMarioState.GetHandler() != &MarioSmall::Die || smallMarioState.GetHandler() != &MarioSmall::DieFall)
		Mario::OnOutOfWorld();
	else
		UnsubscribeToOutOfWorldEvent();
}

void MarioSmall::Die(float delta) {
	time += delta;

	if (time >= 0.75f) {
		time = 0;
		velocity.y = -DEATH_JUMP_SPEED;
		smallMarioState.SetHandler(&MarioSmall::DieFall);
	}
}

void MarioSmall::DieFall(float delta) {
	velocity.y += DEATH_FALL_ACCEL * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	time += delta;
	if (time >= 3.0f)
		Game::QueueFreeAndSwitchScene(parentScene->GetPrevScenePath());
}