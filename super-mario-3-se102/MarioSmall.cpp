#include "MarioSmall.h"
#include "Game.h"
using namespace Entities;

const Mario::AnimationSet MarioSmall::animationSet = Mario::AnimationSet(
	"MarioSIL", "MarioSIR", "MarioSTL", "MarioSTR", "MarioSWL", "MarioSWR", "MarioSJL", "MarioSJR"
);

MarioSmall::MarioSmall(Utils::Vector2<float> position)
	: Mario(position, animationSet, PlayerPowerLevel::SMALL),
	smallMarioState(this),
	died(false)
{
}

void MarioSmall::TakeDamage()
{
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
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	time += delta;
	if (time >= 3.0f)
		Game::QueueFreeAndSwitchScene(parentScene->GetPrevScenePath());
}