#include "Mario.h"
#include "Game.h"
#include "EventHandler.h"
#include "CollisionHandling.h"
#include "EntityConstants.h"
#include "Group.h"
#include "Contains.h"
#include "Scene.h"
#include "PlayerVariables.h"
using namespace Entities;
using namespace Utils;

const float Mario::MAX_WALK_SPEED = 90;
const float Mario::MAX_RUN_SPEED = 180;
const float Mario::RUN_STATE_ANIM_SPEED = 2;
const float Mario::ACCELERATION = 150;
const float Mario::FRICTION_ACCEL = 500;
const float Mario::WAIT_TIME_BEFORE_INCREASE_POWER_METER = 0.75f;
const float Mario::POWER_METER_INCREASE_RATE = 0.125f;
const float Mario::POWER_METER_DECREASE_RATE = 0.25f;
const float Mario::BOUNCE_SPEED = 200;
const float Mario::BOUNCE_SPEED_HOLD_JUMP = BOUNCE_SPEED * 2;
const float Mario::JUMP_SPEED = 325;
const float Mario::JUMP_SPEED_RELASE_EARLY = JUMP_SPEED / 1.75f;
const float Mario::JUMP_SPEED_AFTER_WALK = JUMP_SPEED + 25;
const float Mario::DEATH_JUMP_SPEED = JUMP_SPEED / 1.1f;
const float Mario::DEATH_FALL_ACCEL = EntityConstants::GRAVITY / 1.5f;
const float Mario::WALK_SPEED_REACHED_GOAL_ROULETTE = MAX_WALK_SPEED / 1.5f;
const float Mario::INVINCIBLE_DURATION = 1.5;
const int Mario::FLASH_DURATION = 2;
const float Mario::MAX_MOVE_ANIM_SPEED = 2.0f;
const float Mario::INCREASE_MOVE_ANIM_UNIT = 1 / 30.0f;

Mario::Mario(
	const Utils::Vector2<float>& position, HDirection initialFacingDirection, const MarioAnimationSet& animationSet,
	const std::string& hitboxId, PlayerPowerLevel powerLevel
)
	: Entity::Entity(
		position,
		initialFacingDirection == HDirection::LEFT ? animationSet.idleLeft : animationSet.idleRight,
		hitboxId,
		{ Group::PLAYERS, Group::NOT_AFFECTED_BY_TRANSITION_PAUSE },
		GridType::NONE
	),
	animationSet(animationSet),
	normalHitboxId(hitboxId),
	powerLevel(powerLevel),
	state(EntityState<Mario>(this, &Mario::Fall)),
	updateState(EntityState<Mario>(this, &Mario::NormalUpdate)),
	lastPressedKeyHorizontal(initialFacingDirection == HDirection::LEFT ? DIK_LEFT : DIK_RIGHT),
	inputDir(Vector2<int>(0, initialFacingDirection == HDirection::LEFT ? -1 : 1)),
	time(0),
	powerMeterTime(0),
	walkAnimSpeed(1),
	instantDecrease(false),
	isRunning(false),
	onGround(false),
	runBeforeJump(false)
{
}

void Mario::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Mario::OnCollision, { Group::COLLISION_WALLS });
}

void Mario::Update(float delta) {
	Entity::Update(delta);
	updateState.Update(delta);
}

void Mario::PostUpdate()
{
	Entity::PostUpdate();
	onGround = false;
}

void Mario::NormalUpdate(float delta)
{
	UpdateInputDirection();
	state.Update(delta);

}

void Mario::InvincibilityUpdate(float delta)
{
	NormalUpdate(delta);

	invincibleTime += delta;
	if (invincibleTime >= INVINCIBLE_DURATION) {
		invincibleTime = 0;
		updateState.SetState(&Mario::NormalUpdate);
	}
}

void Mario::Render()
{
	if (IsInvincible())
		InvincibilityRender();
	else
		Entity::Render();
}

void Mario::InvincibilityRender() {
	static const int maxFPS = Game::GetGameSettings().maxFPS;

	if (int(invincibleTime * maxFPS / FLASH_DURATION) % 2)
		Entity::Render();
}

void Mario::TurnInvinsible()
{
	updateState.SetState(&Mario::InvincibilityUpdate);
}

bool Mario::IsInvincible()
{
	return (updateState.GetState() == &Mario::InvincibilityUpdate);
}

void Mario::SwitchState(EntityState<Mario>::StateHandler stateHandler) {
	if (state.GetState() == &Mario::OutOfWorldDeath)
		return;

	state.SetState(stateHandler);

	powerMeterTime = 0;

	if (stateHandler == &Mario::Idle || stateHandler == &Mario::Walk || stateHandler == &Mario::WalkSpeedUp) {
		isRunning = false;
		return;
	}

	if (stateHandler == &Mario::Run) {
		isRunning = true;
		return;
	}

	if (stateHandler == &Mario::Jump) {
		velocity.y = (abs(velocity.x) >= MAX_WALK_SPEED / 2) ? -JUMP_SPEED_AFTER_WALK : -JUMP_SPEED;
		inputDir.y = 1;
		onGround = false;
		restartPointUp.Notify();
		return;
	}

	if (stateHandler == &Mario::Fall) {
		powerMeterTime = 0;
		onGround = false;
		inputDir.y = 1;
		return;
	}

	if (stateHandler == &Mario::BounceUp) {
		velocity.y = (Game::IsKeyDown(DIK_S)) ? -BOUNCE_SPEED_HOLD_JUMP : -BOUNCE_SPEED;
		return;
	}

	if (stateHandler == &Mario::ReachedGoalRouletteFall) {
		SetAnimation(animationSet.fallRight);
		velocity = { 0, 0 };
		return;
	}

	if (stateHandler == &Mario::ReachedGoalRouletteWalkAway) {
		time = 0;
		SetAnimation(animationSet.walkRight, 1.75f);

		parentScene->UnsubscribeToOutOfWorldEvent(this);
		SetDetectable(false);
		UnsubscribeToCollisionEngine();
		velocity = { WALK_SPEED_REACHED_GOAL_ROULETTE, 0 };
		return;
	}
}

void Mario::Idle(float delta)
{
	UpdateDecreasePowerMeter(delta);
	ClipHorizontalPosition();
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	if (!AlmostEqual(velocity.x, 0))
		ApplyFriction(delta);

	UpdateAnimation();

	if (!onGround) {
		SwitchState(&Mario::Fall);
		return;
	}

	if (Game::IsKeyPressed(DIK_S)) {
		SwitchState(&Mario::Jump);
		return;
	}

	if (inputDir.x != 0) {
		if (Game::IsKeyDown(DIK_A))
			SwitchState(&Mario::WalkSpeedUp);
		else
			SwitchState(&Mario::Walk);

		return;
	}
}

void Mario::Walk(float delta)
{
	UpdateDecreasePowerMeter(delta);
	ClipHorizontalPosition();

	if (inputDir.x != 0 && !AlmostEqual(velocity.x, 0) && inputDir.x != Sign(velocity.x))
		ApplyFriction(delta);
	else
		ApplyHorizontalMovement(delta);

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	UpdateAnimation();

	if (Game::IsKeyDown(DIK_A))
		SwitchState(&Mario::WalkSpeedUp);

	else if (!onGround)
		SwitchState(&Mario::Fall);

	else if (inputDir.x == 0)
		SwitchState(&Mario::Idle);

	else if (onGround && Game::IsKeyPressed(DIK_S))
		SwitchState(&Mario::Jump);
}

void Mario::WalkSpeedUp(float delta)
{
	if (inputDir.x != 0)
		if (inputDir.x == Sign(velocity.x))
			UpdateIncreasePowerMeter(delta);
		else {
			powerMeterTime = 0;
			UpdateDecreasePowerMeter(delta);
		}

	ClipHorizontalPosition();

	if (inputDir.x != 0 && !AlmostEqual(velocity.x, 0) && inputDir.x != Sign(velocity.x))
		ApplyFriction(delta);
	else
		ApplyHorizontalMovement(delta);

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	UpdateAnimation();

	if (PlayerVariables::GetPowerMeterLevel() >= PlayerVariables::MAX_POWER_METER_VALUE)
		SwitchState(&Mario::Run);

	if (!Game::IsKeyDown(DIK_A))
		SwitchState(&Mario::Walk);

	if (!onGround)
		SwitchState(&Mario::Fall);

	if (inputDir.x == 0)
		SwitchState(&Mario::Idle);

	if (onGround && Game::IsKeyPressed(DIK_S))
		SwitchState(&Mario::Jump);
}

void Mario::Run(float delta) {
	ClipHorizontalPosition();

	if (inputDir.x != 0 && !AlmostEqual(velocity.x, 0) && inputDir.x != Sign(velocity.x))
		ApplyFriction(delta);
	else
		ApplyHorizontalMovement(delta);

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	UpdateAnimation();

	if (inputDir.x != 0 && inputDir.x != Sign(velocity.x))
		SwitchState(&Mario::WalkSpeedUp);

	if (!Game::IsKeyDown(DIK_A))
		SwitchState(&Mario::Walk);

	if (!onGround)
		SwitchState(&Mario::Fall);

	if (inputDir.x == 0)
		SwitchState(&Mario::Idle);

	if (onGround && Game::IsKeyPressed(DIK_S))
		SwitchState(&Mario::Jump);
}

void Mario::Jump(float delta)
{
	UpdateDecreasePowerMeter(delta);

	ClipHorizontalPosition();

	HandleJumpStateMovement(delta);

	UpdateAnimation();

	if (!Game::IsKeyDown(DIK_S) || velocity.y > 0) {
		velocity.y = max(velocity.y, -JUMP_SPEED_RELASE_EARLY);
		SwitchState(&Mario::Fall);
		return;
	}
}

void Mario::HandleJumpStateMovement(float delta)
{
	if (AlmostEqual(velocity.x, 0) || inputDir.x != 0 && inputDir.x == Sign(velocity.x))
		ApplyHorizontalMovement(delta);
	else
		ApplyFriction(delta);

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}

void Mario::Fall(float delta) {
	UpdateDecreasePowerMeter(delta);

	ClipHorizontalPosition();

	HandleFallStateMovement(delta);

	UpdateAnimation();

	if (!onGround)
		return;

	if (abs(velocity.x) == 0) {
		SwitchState(&Mario::Idle);
		return;
	}

	if (Game::IsKeyDown(DIK_A)) {
		if (PlayerVariables::GetPowerMeterLevel() < PlayerVariables::MAX_POWER_METER_VALUE)
			SwitchState(&Mario::WalkSpeedUp);
		else
			SwitchState(&Mario::Run);
	}

	else
		SwitchState(&Mario::Walk);
}

void Mario::HandleFallStateMovement(float delta)
{
	HandleJumpStateMovement(delta);
}

void Mario::BounceUp(float delta)
{
	if (Game::IsKeyDown(DIK_S))
		Fall(delta);
	else
	{
		velocity.y = max(velocity.y, -JUMP_SPEED_RELASE_EARLY);
		SwitchState(&Mario::Fall);
	}
}

void Mario::UpdateAnimation()
{
	if (state.GetState() == &Mario::Idle) {
		UpdateIdleStateAnimation();
		return;
	}

	if (state.GetState() == &Mario::Walk) {
		walkAnimSpeed = max(walkAnimSpeed - INCREASE_MOVE_ANIM_UNIT, 1.0f);
		if (inputDir.x != 0) {
			if (inputDir.x == Sign(velocity.x))
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight, walkAnimSpeed);
			else
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.turnLeft : animationSet.turnRight);
		}

		return;
	}

	if (state.GetState() == &Mario::WalkSpeedUp) {
		walkAnimSpeed = min(walkAnimSpeed + INCREASE_MOVE_ANIM_UNIT, MAX_MOVE_ANIM_SPEED);
		if (inputDir.x != 0)
			if (inputDir.x == Sign(velocity.x))
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight, walkAnimSpeed);
			else {
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.turnLeft : animationSet.turnRight);
				walkAnimSpeed = 1;
			}

		return;
	}

	if (state.GetState() == &Mario::Run) {
		if (inputDir.x != 0)
			if (inputDir.x == Sign(velocity.x))
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.runLeft : animationSet.runRight);
			else
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.turnLeft : animationSet.turnRight);

		return;
	}

	if (state.GetState() == &Mario::Jump) {
		if (isRunning)
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.runJumpLeft : animationSet.runJumpRight);
		else
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);

		return;
	}

	if (state.GetState() == &Mario::Fall) {
		if (velocity.y < 0) {
			if (isRunning)
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.runJumpLeft : animationSet.runJumpRight);
			else
				SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);

			return;
		}

		if (isRunning)
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.runFallLeft : animationSet.runFallRight);
		else
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.fallLeft : animationSet.fallRight);

		return;
	}
}

void Mario::UpdateIdleStateAnimation()
{
	if (inputDir.y == 1) {
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? animationSet.duckLeft : animationSet.duckRight);
		SetHitbox("HitboxMarioDuck");
		return;
	}

	if (AlmostEqual(velocity.x, 0))
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? animationSet.idleLeft : animationSet.idleRight);

	else
		SetAnimation(lastPressedKeyHorizontal == DIK_LEFT ? animationSet.walkLeft : animationSet.walkRight);

	SetHitbox(normalHitboxId);
}

void Mario::OnNoteBlock(float delta)
{
	UpdateDecreasePowerMeter(delta);

	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
	SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);
}

void Mario::OffNoteBlock(float delta)
{
	UpdateDecreasePowerMeter(delta);

	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
	SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);

	if (onGround)
		SwitchState(velocity.x <= MAX_WALK_SPEED ? &Mario::Walk : &Mario::WalkSpeedUp);
}

void Mario::UpdateIncreasePowerMeter(float delta)
{
	instantDecrease = true;

	if (PlayerVariables::GetPowerMeterLevel() >= PlayerVariables::MAX_POWER_METER_VALUE)
		return;

	powerMeterTime += delta;
	if (powerMeterTime >= WAIT_TIME_BEFORE_INCREASE_POWER_METER + POWER_METER_INCREASE_RATE) {
		PlayerVariables::AddToPowerMeterLevel(1);
		powerMeterTime = WAIT_TIME_BEFORE_INCREASE_POWER_METER;
	}
}

void Mario::UpdateDecreasePowerMeter(float delta)
{
	if (PlayerVariables::GetPowerMeterLevel() <= 0)
		return;

	if (instantDecrease) {
		PlayerVariables::AddToPowerMeterLevel(-1);
		instantDecrease = false;
		return;
	}

	powerMeterTime += delta;
	if (powerMeterTime >= POWER_METER_DECREASE_RATE) {
		PlayerVariables::AddToPowerMeterLevel(-1);
		powerMeterTime = 0;
	}
}

void Mario::ReachedGoalRouletteFall(float delta)
{
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	if (onGround)
		SwitchState(&Mario::ReachedGoalRouletteWalkAway);
}

void Mario::ReachedGoalRouletteWalkAway(float delta)
{
}

void Mario::OutOfWorldDeath(float delta)
{
	time += delta;
	if (time >= 3.0f)
		Game::QueueFreeAndSwitchScene(parentScene->GetPrevScenePath());
}

void Mario::ApplyHorizontalMovement(float delta)
{
	float isHoldingRunButton = Game::IsKeyDown(DIK_A);
	float accelX = ACCELERATION;

	//transition from run to walk
	if (!isHoldingRunButton && abs(velocity.x) > MAX_WALK_SPEED && Sign(velocity.x) == inputDir.x)
		accelX = -accelX;

	else if (isHoldingRunButton && abs(velocity.x) == MAX_RUN_SPEED && Sign(velocity.x) == inputDir.x)
		accelX = 0;

	velocity.x += inputDir.x * accelX * delta;

	if (isHoldingRunButton)
		velocity.x = Clip(velocity.x, -MAX_RUN_SPEED, MAX_RUN_SPEED);

	else if (!isHoldingRunButton && accelX == -ACCELERATION && abs(velocity.x) < MAX_WALK_SPEED)
		velocity.x = MAX_WALK_SPEED * inputDir.x;
}

void Mario::ApplyFriction(float delta) {
	int frictionDirX = -Sign(velocity.x);
	velocity.x += FRICTION_ACCEL * frictionDirX * delta;

	if (Sign(velocity.x) == frictionDirX)
		velocity.x = 0;
}

void Mario::ClipHorizontalPosition()
{
	float dimX = Game::GetGameSettings().gameDimension.width;
	float cameraPosX = parentScene->GetCamera().GetPosition().x;
	float marioOffset = GetHitbox().GetDimension().width + GetHitbox().GetRelativePosition().x;
	position.x = Clip(position.x, cameraPosX, cameraPosX + dimX - marioOffset);
}

Event<>& Mario::GetRestartPointUpEvent()
{
	return restartPointUp;
}

void Mario::Bounce()
{
	SwitchState(&Mario::BounceUp);
}

void Mario::UnsubscribeToCollisionEngine()
{
	CollisionEngine::Unsubscribe(this, &Mario::OnCollision);
}

void Mario::OnCollision(CollisionData data)
{
	const EntityGroups& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y != 0)
			velocity.y = 0;

		if (data.edge.x != 0)
			velocity.x = 0;

		if (data.edge.y == -1)
			onGround = true;
	}

	else if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1) {
		CollisionHandling::Slide(this, data);
		onGround = true;
		velocity.y = 0;
	}
}

void Mario::UpdateInputDirection()
{
	if (Game::IsKeyDown(DIK_RIGHT)) {
		lastPressedKeyHorizontal = DIK_RIGHT;
		inputDir.x = 1;
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		lastPressedKeyHorizontal = DIK_LEFT;
		inputDir.x = -1;
	}
	else
		inputDir.x = 0;

	if (Game::IsKeyDown(DIK_UP)) {
		inputDir.y = -1;
	}
	else if (Game::IsKeyDown(DIK_DOWN)) {
		inputDir.y = 1;
	}
	else
		inputDir.y = 0;
}

HDirection Mario::GetFacingDirection()
{
	if (inputDir.x != 0)
		return inputDir.x < 0 ? HDirection::LEFT : HDirection::RIGHT;
	else
		return lastPressedKeyHorizontal == DIK_LEFT ? HDirection::LEFT : HDirection::RIGHT;
}

void Mario::StartReachedGoalRouletteAnimation()
{
	SwitchState(&Mario::ReachedGoalRouletteFall);
}

void Mario::ShowGoInPipeAnimation()
{
	SetAnimation(animationSet.goInPipe);
}

void Mario::SetOnNoteBlock(bool state)
{
	SwitchState(state ? &Mario::OnNoteBlock : &Mario::OffNoteBlock);
}

void Mario::OnOutOfWorld()
{
	if (position.y < 0)
		return;

	Dimension<float> worldDim = parentScene->GetWorldDimension();
	Dimension<float> marioDim = GetCurrentSpriteDimension();

	velocity = { 0,0 };
	parentScene->UnsubscribeToOutOfWorldEvent(this);
	parentScene->TransitionPause(true);
	SwitchState(&Mario::OutOfWorldDeath);
}

PlayerPowerLevel Mario::GetPowerLevel()
{
	return powerLevel;
}

bool Mario::IsBouncingUp()
{
	return state.GetState() == &Mario::BounceUp;
}

bool Mario::IsJumping()
{
	return state.GetState() == &Mario::Jump;
}

bool Mario::IsFalling()
{
	return state.GetState() == &Mario::Fall;
}