#include "MarioSmall.h"
#include "EntityConstants.h"
#include "Game.h"
#include "Utils.h"
using namespace Entities;
using namespace Utils;

const MarioAnimationSet MarioSmall::animationSet(
	"MarioSIL",
	"MarioSIR",
	"MarioSTL",
	"MarioSTR",
	"MarioSWL",
	"MarioSWR",
	"MarioSRL",
	"MarioSRR",
	"MarioSJL",
	"MarioSJR",
	"MarioSJL",
	"MarioSJR",
	"MarioSIL",
	"MarioSIR",
	"MarioSIHL",
	"MarioSIHR",
	"MarioSHL",
	"MarioSHR"
);

MarioSmall::MarioSmall(Vector2<float> position, HDirection initialFacingDirection)
	: Mario(position, initialFacingDirection, animationSet, "HitboxMarioS", PlayerPowerLevel::SMALL),
	smallMarioState(this, &MarioSmall::Idle),
	died(false)
{
}

void MarioSmall::TakeDamage()
{
	if (died)
		return;

	parentScene->TransitionPause(true);
	SetAnimation("MarioDeath");
	UnsubscribeToCollisionEngine();
	time = 0;
	velocity = { 0, 0 };

	died = true;
	smallMarioState.SetState(&MarioSmall::Die);
}

void MarioSmall::Update(float delta)
{
	if (died) {
		Entity::Update(delta);
		smallMarioState.Update(delta);
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
	if (smallMarioState.GetState() != &MarioSmall::Die || smallMarioState.GetState() != &MarioSmall::DieFall)
		Mario::OnOutOfWorld();
	else
		parentScene->UnsubscribeToOutOfWorldEvent(this);
}

void MarioSmall::HandleIdleStateAnimation()
{
	//same as Mario::HandleIdleStateAnimation but do not allow ducking

	if (AlmostEqual(velocity.x, 0))
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? animationSet.idleLeft : animationSet.idleRight);

	else
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? animationSet.walkLeft : animationSet.walkRight);
}

void MarioSmall::Die(float delta) {
	time += delta;

	if (time >= 0.75f) {
		time = 0;
		velocity.y = -DEATH_JUMP_SPEED;
		smallMarioState.SetState(&MarioSmall::DieFall);
	}
}

void MarioSmall::DieFall(float delta) {
	velocity.y += DEATH_FALL_ACCEL * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	time += delta;
	if (time >= 3.0f)
		Game::QueueFreeAndSwitchScene(parentScene->GetPrevScenePath());
}