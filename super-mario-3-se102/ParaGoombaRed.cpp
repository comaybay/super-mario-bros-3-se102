#include "ParaGoombaRed.h"
#include "Goomba.h"
#include "CollisionHandling.h"
#include "Group.h"
#include "EntityConstants.h"
#include "Scene.h"
#include "Mario.h"
#include "Contains.h"
#include "EntityUtils.h"
using namespace Entities;
using namespace Utils;

const float ParaGoombaRed::TIME_TILL_PREPARE = 0.75f;
const float ParaGoombaRed::PREPARE_JUMP_SPEED = 100;
const int ParaGoombaRed::NUM_OF_PREPARE_JUMPS = 3;
const float ParaGoombaRed::JUMP_SPEED = 240;
const float ParaGoombaRed::FALL_ACCEL = EntityConstants::GRAVITY / 1.2f;
const float ParaGoombaRed::JUMP_FLAP_ANIM_SPEED = 3;

ParaGoombaRed::ParaGoombaRed(const Vector2<float>& position)
	: ParaGoomba(Color::RED, position),
	state(EntityState<ParaGoombaRed>(this, &ParaGoombaRed::MoveAround)),
	jumpCount(0),
	time(0)
{
	SetAnimation(colorCode + "GoombaM");
	FlapDown();
}

void ParaGoombaRed::Update(float delta)
{
	ParaGoomba::Update(delta);

	state.Update(delta);
	velocity.y += FALL_ACCEL * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	UpdateWings(delta);
}

void ParaGoombaRed::MoveAround(float delta) {
	time += delta;

	if (time >= TIME_TILL_PREPARE) {
		time = 0;
		AutoFlap();

		if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
			LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
			velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -Goomba::WALK_SPEED : Goomba::WALK_SPEED;
		}

		state.SetState(&ParaGoombaRed::PrepareToJump);
	}
}

void ParaGoombaRed::PrepareToJump(float delta) {
	if (!onGround)
		return;

	jumpCount++;

	if (jumpCount <= NUM_OF_PREPARE_JUMPS)
		velocity.y -= PREPARE_JUMP_SPEED;
	else
	{
		jumpCount = 0;
		velocity.y -= JUMP_SPEED;

		AutoFlap();
		SetFlapSpeed(JUMP_FLAP_ANIM_SPEED);

		state.SetState(&ParaGoombaRed::Jump);
	}
}

void ParaGoombaRed::Jump(float delta) {
	if (velocity.y < 0)
		return;

	SetFlapSpeed(1.0f);
	state.SetState(&ParaGoombaRed::Fall);
}

void ParaGoombaRed::Fall(float delta) {
	if (!onGround)
		return;

	FlapDown();
	state.SetState(&ParaGoombaRed::MoveAround);
}

