#include "ParaGoombaBrown.h"
#include "EntityUtils.h"
#include "Scene.h"
#include "Group.h"
#include "Goomba.h"
#include "TinyGoomba.h"
#include "EntityConstants.h"

using namespace Entities;
using namespace Utils;

const float ParaGoombaBrown::WAIT_TIME = 0.5f;
const float ParaGoombaBrown::FALL_ACCEL = EntityConstants::GRAVITY / 2.0f;
const float ParaGoombaBrown::TURN_ACCEL = 500;
const float ParaGoombaBrown::MAX_TURN_SPEED = 90;
const float ParaGoombaBrown::FLY_SPEED = 130;
const float ParaGoombaBrown::FLY_OFFSET_Y = 4;
const float ParaGoombaBrown::FLY_WAVE_Y_DUARTION = 0.5f;
const float ParaGoombaBrown::DISTANCE_ABOVE_PLAYER = Constants::TILE_SIZE * 4.5f;
const float ParaGoombaBrown::DISTANCE_BEFORE_TURN = Constants::TILE_SIZE * 3.5f;
const float ParaGoombaBrown::NUMBER_OF_DROPS = 4;
const float ParaGoombaBrown::DROP_TINY_GOOMBA_RATE = 0.5f;

ParaGoombaBrown::ParaGoombaBrown(const Utils::Vector2<float>& position)
	: ParaGoomba(Color::BROWN, position),
	targetPlayer(nullptr),
	dropCount(0),
	turnDir(0),
	time(0),
	flyBasePosY(0),
	xStartTurn(0),
	state(this, &ParaGoombaBrown::MoveAround)
{
}

void ParaGoombaBrown::Update(float delta)
{
	ParaGoomba::Update(delta);

	state.Update(delta);
	UpdateWings(delta);
}

void ParaGoombaBrown::OnPlayerDestroy(LPEntity _)
{
	targetPlayer = nullptr;
}

void ParaGoombaBrown::MoveAround(float delta)
{
	velocity.y += FALL_ACCEL * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	time += delta;
	if (time >= WAIT_TIME) {
		time = 0;
		FlapUpWings();
		velocity.x = GetVelocityXTowardPlayer();
		state.SetState(&ParaGoombaBrown::PrepareToFly);
	}
}

void ParaGoombaBrown::PrepareToFly(float delta) {
	velocity.y += FALL_ACCEL * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	time += delta;
	if (time >= WAIT_TIME) {
		time = 0;

		if (parentScene->IsEntityGroupEmpty(Group::PLAYERS))
			state.SetState(&ParaGoombaBrown::MoveAround);

		AutoFlapWings();
		SetWingFlapSpeed(2);
		velocity.x = GetVelocityXTowardPlayer();
		bool abovePlayer = EntityUtils::IsAboveOf(targetPlayer, this);
		bool higherThanFlyDistance = targetPlayer->GetPosition().y - position.y >= DISTANCE_ABOVE_PLAYER;

		velocity.y = abovePlayer && higherThanFlyDistance ? FLY_SPEED : -FLY_SPEED;

		state.SetState(&ParaGoombaBrown::FlyUp);
	}
}

void ParaGoombaBrown::FlyUp(float delta) {
	if (targetPlayer == nullptr)
		return;

	if (velocity.y < 0 && targetPlayer->GetPosition().y - position.y < DISTANCE_ABOVE_PLAYER)
		return;

	if (velocity.y > 0 && targetPlayer->GetPosition().y - position.y > DISTANCE_ABOVE_PLAYER)
		return;

	velocity.y = 0;
	xStartTurn = position.x;
	flyBasePosY = position.y;
	turnDir = EntityUtils::IsOnLeftSideOf(this, targetPlayer) ? -1 : 1;
	state.SetState(&ParaGoombaBrown::Turn);
}

void ParaGoombaBrown::Turn(float delta) {
	position.y = flyBasePosY + EntityUtils::Wave(-FLY_OFFSET_Y, FLY_OFFSET_Y, FLY_WAVE_Y_DUARTION, 0.5f, time);

	velocity.x += turnDir * TURN_ACCEL * delta;
	velocity.x = Clip(velocity.x, -MAX_TURN_SPEED, MAX_TURN_SPEED);

	if (targetPlayer && abs(position.x - xStartTurn) >= DISTANCE_BEFORE_TURN) {
		xStartTurn = position.x;
		turnDir = EntityUtils::IsOnLeftSideOf(this, targetPlayer) ? -1 : 1;
	}

	time += delta;
	if (time < DROP_TINY_GOOMBA_RATE * (dropCount + 1))
		return;

	parentScene->AddEntity(new TinyGoomba(position));
	dropCount++;

	if (dropCount == NUMBER_OF_DROPS) {
		dropCount = 0;
		time = 0;
		velocity.x = Goomba::WALK_SPEED * turnDir;
		FlapUpWings();
		state.SetState(&ParaGoombaBrown::Fall);
		return;
	}
}

void ParaGoombaBrown::Fall(float delta)
{
	velocity.y += FALL_ACCEL * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	if (onGround) {
		FlapDownWings();
		state.SetState(&ParaGoombaBrown::MoveAround);
	}
}

float ParaGoombaBrown::GetVelocityXTowardPlayer()
{
	if (targetPlayer == nullptr && parentScene->IsEntityGroupEmpty(Group::PLAYERS))
		return -Goomba::WALK_SPEED;

	targetPlayer = parentScene->GetEntityOfGroup(Group::PLAYERS);
	targetPlayer->GetDestroyEvent().Subscribe(this, &ParaGoombaBrown::OnPlayerDestroy);

	bool playerOnLeftSide = EntityUtils::IsOnLeftSideOf(this, targetPlayer);
	return playerOnLeftSide ? -Goomba::WALK_SPEED : Goomba::WALK_SPEED;
}
