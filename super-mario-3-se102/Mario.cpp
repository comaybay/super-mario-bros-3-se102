#include "Mario.h"
#include "Game.h"
#include "EventHandler.h"
#include "CollisionHandling.h"
#include "EntityConstants.h"
#include "Group.h"
#include "Contains.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

const float Mario::MAX_WALK_SPEED = 100;
const float Mario::MAX_RUN_SPEED = 150;
const float Mario::RUN_STATE_ANIM_SPEED = 2;
const float Mario::ACCELERATION = 350;
const float Mario::BOUNCE_SPEED = 200;
const float Mario::BOUNCE_SPEED_HOLD_JUMP = BOUNCE_SPEED * 2;
const float Mario::JUMP_SPEED = 325;
const float Mario::JUMP_SPEED_RELASE_EARLY = JUMP_SPEED / 1.75f;
const float Mario::JUMP_SPEED_AFTER_WALK = JUMP_SPEED + 25;
const float Mario::DEATH_JUMP_SPEED = JUMP_SPEED / 1.1f;
const float Mario::DEATH_FALL_ACCEL = EntityConstants::GRAVITY / 1.5f;
const float Mario::WALK_SPEED_REACHED_GOAL_ROULETTE = MAX_WALK_SPEED / 1.5f;
const float Mario::INVINCIBLE_DURATION = 1;
const int Mario::FLASH_DURATION = 2;
const float Mario::MAX_MOVE_ANIM_SPEED = 1.5f;
const float Mario::MOVE_ANIM_SPEED_INCREASE_UNIT = 1 / 60.0f;

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
	powerLevel(powerLevel),
	marioState(EntityState<Mario>(this, &Mario::Idle)),
	updateState(EntityState<Mario>(this, &Mario::NormalUpdate)),
	lastPressedKeyHorizontal(initialFacingDirection == HDirection::LEFT ? DIK_LEFT : DIK_RIGHT),
	dir(Vector2<int>(0, initialFacingDirection == HDirection::LEFT ? -1 : 1)),
	time(0),
	moveAnimSpeed(1),
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

void Mario::NormalUpdate(float delta)
{
	UpdateHorizontalDirection();
	marioState.Update(delta);

	onGround = false;
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
	if (marioState.GetState() == &Mario::OutOfWorldDeath)
		return;

	marioState.SetState(stateHandler);

	if (stateHandler == &Mario::Jump) {
		velocity.y = (abs(velocity.x) >= MAX_WALK_SPEED / 2) ? -JUMP_SPEED_AFTER_WALK : -JUMP_SPEED;
		dir.y = 1;
		onGround = false;
		restartPointUp.Notify();
		return;
	}

	if (stateHandler == &Mario::Fall) {
		onGround = false;
		dir.y = 1;
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
		velocity.x = WALK_SPEED_REACHED_GOAL_ROULETTE;
		return;
	}
}

void Mario::Idle(float delta)
{
	ClipHorizontalPosition();
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	if (velocity.x != 0)
		ApplyFriction(delta);

	if (velocity.x == 0)
		SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.idleLeft : animationSet.idleRight);
	else
		SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight);

	if (!onGround)
		SwitchState(&Mario::Fall);

	if (Game::IsKeyPressed(DIK_S))
		SwitchState(&Mario::Jump);

	else if (dir.x != 0) {
		if (Game::IsKeyDown(DIK_A))
			SwitchState(&Mario::Run);
		else
			SwitchState(&Mario::Walk);
	}
}

void Mario::Walk(float delta)
{
	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	moveAnimSpeed = max(moveAnimSpeed - MOVE_ANIM_SPEED_INCREASE_UNIT, 1.0f);
	if (dir.x != 0) {
		if (dir.x == Sign(velocity.x))
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight, moveAnimSpeed);
		else {
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.turnLeft : animationSet.turnRight, moveAnimSpeed);
			moveAnimSpeed = 1;
		}
	}

	if (Game::IsKeyDown(DIK_A))
		SwitchState(&Mario::Run);

	else if (!onGround)
		SwitchState(&Mario::Fall);

	else if (dir.x == 0)
		SwitchState(&Mario::Idle);

	else if (onGround && Game::IsKeyPressed(DIK_S))
		SwitchState(&Mario::Jump);
}

void Mario::Run(float delta)
{
	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	moveAnimSpeed = min(moveAnimSpeed + MOVE_ANIM_SPEED_INCREASE_UNIT, MAX_MOVE_ANIM_SPEED);
	if (dir.x != 0) {
		if (dir.x == Sign(velocity.x))
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight, moveAnimSpeed);
		else {
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.turnLeft : animationSet.turnRight, moveAnimSpeed);
			moveAnimSpeed = 1;
		}
	}

	if (!Game::IsKeyDown(DIK_A))
		SwitchState(&Mario::Walk);

	if (!onGround)
		SwitchState(&Mario::Fall);

	if (dir.x == 0)
		SwitchState(&Mario::Idle);

	if (onGround && Game::IsKeyPressed(DIK_S))
		SwitchState(&Mario::Jump);
}

void Mario::Jump(float delta)
{
	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	if (dir.x == 0 && velocity.x != 0)
		ApplyFriction(delta);

	SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);

	if (!Game::IsKeyDown(DIK_S) || velocity.y > 0) {
		velocity.y = max(velocity.y, -JUMP_SPEED_RELASE_EARLY);
		SwitchState(&Mario::Fall);
		return;
	}
}

void Mario::Fall(float delta) {
	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	if (dir.x == 0 && velocity.x != 0)
		ApplyFriction(delta);

	if (velocity.y < 0)
		SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);
	else
		SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.fallLeft : animationSet.fallRight);

	if (!onGround)
		return;

	if (abs(velocity.x) == 0) {
		SwitchState(&Mario::Idle);
		return;
	}

	if (Game::IsKeyDown(DIK_A))
		SwitchState(&Mario::Run);
	else
		SwitchState(&Mario::Walk);
}

void Mario::BounceUp(float delta)
{
	Fall(delta);
}

void Mario::OnNoteBlock(float delta)
{
	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
	SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);
}

void Mario::OffNoteBlock(float delta)
{
	ClipHorizontalPosition();
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
	SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);

	if (onGround)
		SwitchState(velocity.x <= MAX_WALK_SPEED ? &Mario::Walk : &Mario::Run);
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
	if (!isHoldingRunButton && abs(velocity.x) > MAX_WALK_SPEED && Sign(velocity.x) == dir.x)
		accelX = -ACCELERATION;

	else if (isHoldingRunButton && abs(velocity.x) == MAX_RUN_SPEED && Sign(velocity.x) == dir.x)
		accelX = 0;

	velocity.x += dir.x * accelX * delta;

	if (isHoldingRunButton)
		velocity.x = Clip(velocity.x, -MAX_RUN_SPEED, MAX_RUN_SPEED);

	else if (!isHoldingRunButton && accelX == -ACCELERATION && abs(velocity.x) < MAX_WALK_SPEED)
		velocity.x = MAX_WALK_SPEED * dir.x;
}

void Mario::ApplyFriction(float delta) {
	int frictionDirX = -Sign(velocity.x);
	velocity.x += ACCELERATION * frictionDirX * delta;

	if (Sign(velocity.x) == frictionDirX)
		velocity.x = 0;
}

void Mario::ClipHorizontalPosition()
{
	Dimension<float> worldDim = parentScene->GetWorldDimension();
	Dimension<float> marioDim = GetCurrentSpriteDimension();
	position.x = Clip(position.x, 0.0f, worldDim.width - marioDim.width);
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

void Mario::UpdateHorizontalDirection()
{
	if (Game::IsKeyDown(DIK_RIGHT)) {
		lastPressedKeyHorizontal = DIK_RIGHT;
		dir.x = 1;
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		lastPressedKeyHorizontal = DIK_LEFT;
		dir.x = -1;
	}
	else {
		dir.x = 0;
	}
}

HDirection Mario::GetFacingDirection()
{
	if (dir.x != 0)
		return dir.x < 0 ? HDirection::LEFT : HDirection::RIGHT;
	else
		return lastPressedKeyHorizontal == DIK_LEFT ? HDirection::LEFT : HDirection::RIGHT;
}

void Mario::StartReachedGoalRouletteAnimation()
{
	SwitchState(&Mario::ReachedGoalRouletteFall);
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
