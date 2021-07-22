#include "MarioRaccoon.h"
#include "MarioTransition.h"
#include "Scene.h"
#include "PlayerVariables.h"
#include "Game.h"
#include "EntityConstants.h"
#include "MarioSpinAttack.h"

using namespace Entities;
using namespace Utils;

const MarioAnimationSet MarioRaccoon::animationSet(
	"MarioRIL",
	"MarioRIR",
	"MarioRTL",
	"MarioRTR",
	"MarioRWL",
	"MarioRWR",
	"MarioRRL",
	"MarioRRR",
	"MarioRJL",
	"MarioRJR",
	"MarioRRJL",
	"MarioRRJR",
	"MarioRFL",
	"MarioRFR",
	"MarioRRFL",
	"MarioRRFR",
	"MarioRDL",
	"MarioRDR",
	"MarioRIHL",
	"MarioRIHR",
	"MarioRHL",
	"MarioRHR"
);

const float MarioRaccoon::FLY_FLAP_SPEED = 100;
const float MarioRaccoon::FALL_FLAP_SPEED = 50;
const float MarioRaccoon::FLAP_DURATION = 0.3f;
const float MarioRaccoon::POWER_METER_DURATION = 4;

MarioRaccoon::MarioRaccoon(const Vector2<float>& position, HDirection direction)
	: Mario(position, direction, animationSet, "HitboxMarioB", PlayerPowerLevel::TANOOKI),
	flapState(this, &MarioRaccoon::WaitFlap),
	spinState(this, &MarioRaccoon::WaitSpin),
	time(0),
	powerMeterTime(0),
	jumpCount(0),
	isOnFlyFlapAnim(false),
	isOnFallFlapAnim(false),
	isUsingPowerMeter(false)
{
}

void MarioRaccoon::Update(float delta)
{
	Mario::Update(delta);
	spinState.Update(delta);

	if (isUsingPowerMeter)
		PlayerVariables::SetPowerMeter(PlayerVariables::MAX_POWER_METER_VALUE);

	if (isOnFlyFlapAnim && animation->Finished())
		isOnFlyFlapAnim = false;

	if (isOnFallFlapAnim && animation->Finished())
		isOnFallFlapAnim = false;

	flapState.Update(delta);
}

void MarioRaccoon::TakeDamage()
{
	parentScene->AddEntity(new MarioTransition(this, PlayerPowerLevel::BIG));
}

void MarioRaccoon::WaitFlap(float delta)
{
	time += delta;

	if (time < 0.5f)
		return;

	bool pressedJumpWhenOnAir = (IsJumping() || IsFalling() || IsBouncingUp()) && Game::IsKeyPressed(DIK_S);

	if (PlayerVariables::GetPowerMeterLevel() >= PlayerVariables::MAX_POWER_METER_VALUE && pressedJumpWhenOnAir)
	{
		time = 0;
		jumpCount++;
		powerMeterTime = 0;
		isUsingPowerMeter = true;
		flapState.SetState(&MarioRaccoon::FlyCountDown);
		return;
	}

	if (pressedJumpWhenOnAir) {
		time = 0;
		flapState.SetState(&MarioRaccoon::WaitFallFlap);
	}
}

void MarioRaccoon::WaitFallFlap(float delta) {
	if (onGround) {
		time = 0;
		flapState.SetState(&MarioRaccoon::WaitFlap);
		return;
	}

	if (Game::IsKeyPressed(DIK_S)) {
		isOnFallFlapAnim = true;

		if (spinState.GetState() != &MarioRaccoon::Spin)
			animation->SetFrame(0);

		velocity.y = FALL_FLAP_SPEED;
		time = 0;
		flapState.SetState(&MarioRaccoon::FallFlap);
	}
}

void MarioRaccoon::FallFlap(float delta)
{
	if (Game::IsKeyPressed(DIK_S)) {
		time = 0;
		velocity.y = FALL_FLAP_SPEED;
		isOnFallFlapAnim = true;

		if (spinState.GetState() != &MarioRaccoon::Spin)
			animation->SetFrame(0);

		flapState.SetState(&MarioRaccoon::FallFlap);
		return;
	}

	time += delta;
	if (onGround || time >= FLAP_DURATION) {
		isOnFallFlapAnim = false;
		time = 0;
		flapState.SetState(&MarioRaccoon::WaitFallFlap);
	}
}

void MarioRaccoon::FlyFlap(float delta) {
	powerMeterTime += delta;
	if (powerMeterTime >= POWER_METER_DURATION) {
		PlayerVariables::SetPowerMeter(0);
		flapState.SetState(&MarioRaccoon::WaitFlap);
		return;
	}

	if (Game::IsKeyPressed(DIK_S)) {
		time = 0;
		velocity.y = -FLY_FLAP_SPEED;
		isOnFlyFlapAnim = true;

		if (spinState.GetState() != &MarioRaccoon::Spin)
			animation->SetFrame(0);

		flapState.SetState(&MarioRaccoon::FlyFlap);
		return;
	}

	time += delta;
	if (onGround || time >= FLAP_DURATION) {
		isOnFlyFlapAnim = false;
		flapState.SetState(&MarioRaccoon::FlyCountDown);
	}
}

void MarioRaccoon::FlyCountDown(float delta)
{
	powerMeterTime += delta;

	if (powerMeterTime >= POWER_METER_DURATION) {
		time = 0;
		PlayerVariables::SetPowerMeter(0);
		flapState.SetState(&MarioRaccoon::WaitFlap);
		isUsingPowerMeter = false;
		return;
	}

	if (onGround) {
		isOnFlyFlapAnim = false;
		jumpCount = 0;
	}

	if (!Game::IsKeyPressed(DIK_S))
		return;

	jumpCount++;

	//double jump
	if (jumpCount >= 2) {
		DebugOut("FALP AT COUNTED DOWN\n");
		velocity.y = -FLY_FLAP_SPEED;
		time = 0;
		isOnFlyFlapAnim = true;

		if (spinState.GetState() != &MarioRaccoon::Spin)
			animation->SetFrame(0);

		flapState.SetState(&MarioRaccoon::FlyFlap);
	}
}

void MarioRaccoon::UpdateAnimation()
{
	if (spinState.GetState() == &MarioRaccoon::Spin)
		return;

	if (onGround || flapState.GetState() != &MarioRaccoon::FlyFlap && flapState.GetState() != &MarioRaccoon::FallFlap)
		Mario::UpdateAnimation();

	if (isOnFlyFlapAnim) {
		int currentFrame = animation->GetCurrentFrame();
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? "MarioRJumpFlapL" : "MarioRJumpFlapR");
		animation->SetFrame(currentFrame);
		return;
	}

	if (isOnFallFlapAnim) {
		int currentFrame = animation->GetCurrentFrame();
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? "MarioRFallFlapL" : "MarioRFallFlapR");
		animation->SetFrame(currentFrame);
		return;
	}
}

void MarioRaccoon::HandleJumpStateMovement(float delta)
{
	if (AlmostEqual(velocity.x, 0) || inputDir.x == 0 || inputDir.x != 0 && inputDir.x == Sign(velocity.x))
		ApplyHorizontalMovement(delta);
	else
		ApplyFriction(delta);

	if (flapState.GetState() != &MarioRaccoon::FlyFlap && flapState.GetState() != &MarioRaccoon::FallFlap) {
		velocity.y += EntityConstants::GRAVITY * delta;
		velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
	}
}

void MarioRaccoon::HandleFallStateMovement(float delta)
{
	HandleJumpStateMovement(delta);
}

void MarioRaccoon::WaitSpin(float delta)
{
	if (Game::IsKeyPressed(DIK_A)) {
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? "MarioRSpinL" : "MarioRSpinR");
		parentScene->AddEntity(new MarioSpinAttack(this, HDirection::LEFT));
		parentScene->AddEntity(new MarioSpinAttack(this, HDirection::RIGHT));
		spinState.SetState(&MarioRaccoon::Spin);
	}
}

void MarioRaccoon::Spin(float delta) {
	if (animation->Finished())
		spinState.SetState(&MarioRaccoon::WaitSpin);
}