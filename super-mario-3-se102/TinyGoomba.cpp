#include "TinyGoomba.h"
#include "Group.h"
#include "EntityUtils.h"
#include "Game.h"

using namespace Entities;
using namespace Utils;

const float TinyGoomba::FALL_SPEED = 45;
const float TinyGoomba::WAVE_FALL_OFFSET_X = 8;
const float TinyGoomba::WAVE_FALL_X_DURATION = 1.5f;
const float TinyGoomba::WAVE_OFFSET_X = 8;
const float TinyGoomba::WAVE_X_DURATION = 0.5f;
const float TinyGoomba::WAVE_OFFSET_Y = 4;
const float TinyGoomba::WAVE_Y_DURATION = 2.0f;

const float TinyGoomba::PLAYER_FORCE_DOWN_SPEED = 200;
const float TinyGoomba::MAX_HOLD_JUMP_TIME = 0.1f;
const float TinyGoomba::NUMBER_OF_SHAKE_OFFS = 5;

TinyGoomba::TinyGoomba(const Vector2<float>& position)
	: Entity(position, "TinyGoomba", "HitboxTinyGoomba", { "TinyGoombas", Group::ENEMIES }, GridType::NONE),
	state(this, &TinyGoomba::Fall),
	initialX(position.x),
	targetPlayer(nullptr),
	lockHold(true),
	playerHoldTime(0),
	shakeOffCount(0),
	time(0),
	fallOffMovement(nullptr)
{
	velocity.y = FALL_SPEED;
}

void TinyGoomba::OnReady()
{
	CollisionEngine::Subscribe(this, &TinyGoomba::OnCollision, { Group::PLAYERS });
}

void TinyGoomba::Update(float delta)
{
	float oldPosX = position.x;

	position.y += velocity.y * delta;
	state.Update(delta);

	//since posiiton.x is manually assign, this velocity.x will not be used for calculating position
	//but for collision detection (this is a limittion of current collision engine)
	int sign = Sign(position.x - oldPosX);
	velocity.x = sign * 100;
}

void TinyGoomba::Fall(float delta)
{
	time += delta;
	position.x = initialX + EntityUtils::Wave(-WAVE_FALL_OFFSET_X, WAVE_FALL_OFFSET_X, WAVE_FALL_X_DURATION, 0.5f, time);
}

void TinyGoomba::Follow(float delta) {
	if (targetPlayer == nullptr) {
		if (parentScene->IsEntityGroupEmpty(Group::PLAYERS))
			return;

		targetPlayer = parentScene->GetEntityOfGroup(Group::PLAYERS);
		targetPlayer->GetDestroyEvent().Subscribe(this, &TinyGoomba::OnPlayerDestroy);
	}

	time += delta;
	position = targetPlayer->GetPosition() + (targetPlayer->GetCurrentSpriteDimension() / 2) -
		(GetCurrentSpriteDimension() / 2) - GetHitbox().GetRelativePosition() / 2;

	position.x += EntityUtils::Wave(-WAVE_OFFSET_X, WAVE_OFFSET_X, WAVE_X_DURATION, 0.5f, time);
	position.y += EntityUtils::Wave(-WAVE_OFFSET_Y, WAVE_OFFSET_Y, WAVE_Y_DURATION, 0.5f, time);

	if (lockHold && Game::IsKeyPressed(DIK_S)) {
		lockHold = false;
		return;
	}

	if (!lockHold) {
		playerHoldTime += delta;

		if (Game::IsKeyDown(DIK_S) && playerHoldTime < MAX_HOLD_JUMP_TIME)
			return;

		if (playerHoldTime < MAX_HOLD_JUMP_TIME)
			shakeOffCount++;

		playerHoldTime = 0;
		lockHold = true;
		targetPlayer->SetVelocity({ targetPlayer->GetVelocity().x, PLAYER_FORCE_DOWN_SPEED });

		if (shakeOffCount == NUMBER_OF_SHAKE_OFFS) {
			HDirection dir = targetPlayer->GetVelocity().x > 0 ? HDirection::LEFT : HDirection::RIGHT;
			SetAnimation(dir == HDirection::LEFT ? "TinyGoombaKOL" : "TinyGoombaKOR");

			fallOffMovement = new MovementKnockedOver(this, dir);
			state.SetState(&TinyGoomba::FallOff);
		}
	}


}

void TinyGoomba::FallOff(float delta) {
	fallOffMovement->Update(delta);

	if (fallOffMovement->Finished())
		parentScene->QueueFree(this);
}


void TinyGoomba::OnCollision(CollisionData data)
{
	velocity.y = 0;
	targetPlayer = data.who;
	targetPlayer->GetDestroyEvent().Subscribe(this, &TinyGoomba::OnPlayerDestroy);
	SetDetectable(false);
	CollisionEngine::Unsubscribe(this, &TinyGoomba::OnCollision);
	state.SetState(&TinyGoomba::Follow);
}

void TinyGoomba::OnPlayerDestroy(LPEntity _)
{
	targetPlayer = nullptr;
}



