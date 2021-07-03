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

Mario::AnimationSet::AnimationSet(const std::string& idleLeft, const std::string& idleRight, const std::string& turnLeft,
	const std::string& turnRight, const std::string& walkLeft, const std::string& walkRight, const std::string& jumpLeft, const std::string& jumpRight)
	: idleLeft(idleLeft), idleRight(idleRight), turnLeft(turnLeft), turnRight(turnRight),
	walkLeft(walkLeft), walkRight(walkRight), jumpLeft(jumpLeft), jumpRight(jumpRight)
{
}

const float Mario::MAX_FALL_SPEED = EntityConstants::MAX_FALL_SPEED;
const float Mario::MAX_WALK_SPEED = 100;
const float Mario::MAX_RUN_SPEED = 150;
const Vector2<float> Mario::ACCELERATION = Vector2<float>(350, 740);
const float Mario::BOUNCE_SPEED = 200;
const float Mario::BOUNCE_SPEED_HOLD_JUMP = BOUNCE_SPEED * 2;
const float Mario::JUMP_SPEED = 310;
const float Mario::JUMP_SPEED_RELASE_EARLY = JUMP_SPEED / 1.75;
const float Mario::JUMP_SPEED_AFTER_MAX_WALK_SPEED = JUMP_SPEED + 30;
const float Mario::DEATH_JUMP_SPEED = JUMP_SPEED / 1.1;
const float Mario::DEATH_FALL_ACCEL = ACCELERATION.y / 1.5;

Mario::Mario(const Utils::Vector2<float>& position, const AnimationSet& animationSet, PlayerPowerLevel powerLevel) :
	Entity::Entity(position, "MarioSIR", "HitboxMarioS", Group::PLAYER, GridType::NONE),
	animationSet(animationSet),
	powerLevel(powerLevel),
	marioState(EntityState<Mario>(this, &Mario::Idle)),
	lastPressedKeyHorizontal(DIK_RIGHT),
	dir(Vector2<float>(0, 1)),
	time(0),
	onGround(false),
	runBeforeJump(false)
{
}

void Mario::OnReady()
{
	CollisionEngine::Subscribe(this, &Mario::OnCollision, { Group::COLLISION_WALLS });
}

Event<>& Mario::GetRestartPointUpEvent()
{
	return restartPointUp;
}

void Mario::Bounce()
{
	SwitchState(&Mario::BounceUp);
}

void Mario::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y != 0.0f)
			velocity.y = 0;

		if (data.edge.x != 0.0f)
			velocity.x = 0;

		if (data.edge.y == -1.0f)
			onGround = true;
	}

	else if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1.0f) {
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

void Mario::UnsubscribeToCollisionEngine()
{
	CollisionEngine::Unsubscribe(this, &Mario::OnCollision);
}

void Mario::Update(float delta) {
	Entity::Update(delta);

	UpdateHorizontalDirection();
	marioState.Handle(delta);

	onGround = false;
}

PlayerPowerLevel Entities::Mario::GetPowerLevel()
{
	return powerLevel;
}

void Mario::SwitchState(EntityState<Mario>::Handler stateHandler) {
	marioState.SetHandler(stateHandler);

	if (stateHandler == &Mario::Jump) {
		velocity.y = (abs(velocity.x) == MAX_WALK_SPEED) ? -JUMP_SPEED_AFTER_MAX_WALK_SPEED : -JUMP_SPEED;
		dir.y = 1;
		onGround = false;
		restartPointUp.Notify();
	}

	else if (stateHandler == &Mario::Fall) {
		onGround = false;
		dir.y = 1;
	}

	else if (stateHandler == &Mario::BounceUp)
		velocity.y = (Game::IsKeyDown(DIK_S)) ? -BOUNCE_SPEED_HOLD_JUMP : -BOUNCE_SPEED;
}

void Mario::Idle(float delta)
{
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

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
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x != 0) {
		if (dir.x == Sign(velocity.x))
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight);
		else
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.turnLeft : animationSet.turnRight);
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
	ApplyHorizontalMovement(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x != 0) {
		if (dir.x == Sign(velocity.x))
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight);
		else
			SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.turnLeft : animationSet.turnRight);
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
	ApplyHorizontalMovement(delta);
	velocity.y += ACCELERATION.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x == 0 && velocity.x != 0)
		ApplyFriction(delta);

	SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);

	if (!Game::IsKeyDown(DIK_S) || velocity.y > 0) {
		velocity.y = max(velocity.y, -JUMP_SPEED_RELASE_EARLY);
		SwitchState(&Mario::Fall);
		return;
	}
}

void Mario::BounceUp(float delta)
{
	//same as fall
	Fall(delta);
}

void Mario::Fall(float delta) {
	ApplyHorizontalMovement(delta);
	velocity.y += ACCELERATION.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x == 0 && velocity.x != 0)
		ApplyFriction(delta);

	SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.jumpLeft : animationSet.jumpRight);

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

void Mario::ApplyHorizontalMovement(float delta)
{
	float isHoldingRunButton = Game::IsKeyDown(DIK_A);
	float accelX = ACCELERATION.x;

	//transition from run to walk
	if (!isHoldingRunButton && abs(velocity.x) > MAX_WALK_SPEED && Sign(velocity.x) == dir.x)
		accelX = -ACCELERATION.x;

	else if (isHoldingRunButton && abs(velocity.x) == MAX_RUN_SPEED && Sign(velocity.x) == dir.x)
		accelX = 0;

	velocity.x += dir.x * accelX * delta;

	if (isHoldingRunButton)
		velocity.x = Clip(velocity.x, -MAX_RUN_SPEED, MAX_RUN_SPEED);

	else if (!isHoldingRunButton && accelX == -ACCELERATION.x && abs(velocity.x) < MAX_WALK_SPEED)
		velocity.x = MAX_WALK_SPEED * dir.x;
}

void Mario::ApplyFriction(float delta) {
	//apply friction to slow mario down
	int frictionDirX = -Sign(velocity.x);
	velocity.x += ACCELERATION.x * frictionDirX * delta;

	//stop mario
	if (Sign(velocity.x) == frictionDirX)
		velocity.x = 0;
}

