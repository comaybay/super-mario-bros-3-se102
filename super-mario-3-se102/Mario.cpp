#include "Entities.h"
#include "Game.h"
#include "EventHandler.h"
#include "CollisionHandling.h"
#include "Groups.h"
using namespace Entities;
using namespace Utils;

const float Mario::MAX_FALL_SPEED = 230;
const float Mario::MAX_WALK_SPEED = 100;
const float Mario::MAX_RUN_SPEED = 150;
const Vector2<float> Mario::ACCELERATION = Vector2<float>(350, 560);
const float Mario::JUMP_SPEED = 270;
const float Mario::JUMP_SPEED_RELASE_EARLY = JUMP_SPEED / 1.75;
const float Mario::JUMP_SPEED_AFTER_MAX_WALK_SPEED = 290;
const float Mario::DEATH_JUMP_SPEED = JUMP_SPEED * 1.25;

const std::string Mario::AnimationSet::DEATH = "MarioDie";

Mario::Mario(Vector2<float> position) :
	Entity::Entity(position, "MarioSIR", Groups::PLAYER, GridType::NONE),
	state(EntityState<Mario>(this)),
	lastPressedKeyHorizontal(DIK_RIGHT),
	dir(Vector2<float>(0, 1)),
	time(0),
	onGround(false),
	powerLevel(PowerLevel::SMALL)
{
	state.SetHandler(&Mario::Idle);
	animationSet = GetAnimationSetByPowerLevel(powerLevel);
	CollisionEngine::Subscribe(this, &Mario::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES });
}

void Mario::SetPowerLevel(Mario::PowerLevel level)
{
	powerLevel = level;
	animationSet = GetAnimationSetByPowerLevel(powerLevel);
}

void Mario::OnCollision(CollisionData data)
{
	std::vector<std::string> groups = data.who->GetEntityGroups();

	if (VectorHas(Groups::COLLISION_WALLS_TYPE_1, groups))
		WallSlide(data);


	else if (VectorHas(Groups::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1.0f)
		WallSlide(data);

	else if (VectorHas(Groups::ENEMIES, groups)) {
		GetParentScene()->PlayerDeath();
		SwitchState(&Mario::DieWait);
	}
}

void Mario::WallSlide(CollisionData& data)
{
	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f)
		velocity.y = 0;

	if (data.edge.y == -1.0f)
		onGround = true;

	if (data.edge.x != 0.0f)
		velocity.x = 0;
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

Mario::AnimationSet Mario::GetAnimationSetByPowerLevel(Mario::PowerLevel powerLevel)
{
	switch (powerLevel) {
	case PowerLevel::SMALL:
		return AnimationSet{ "MarioSIL", "MarioSIR", "MarioSTL", "MarioSTR", "MarioSML", "MarioSMR", "MarioSJL" , "MarioSJR" };
	case PowerLevel::BIG:
		return AnimationSet{ "MarioBIL", "MarioBIR", "MarioBTL", "MarioBTR", "MarioBML", "MarioBMR", "MarioBJL" , "MarioBJR" };
	}
}

void Mario::Update(float delta) {
	Entity::Update(delta);

	UpdateHorizontalDirection();
	state.Handle(delta);
}

void Mario::SwitchState(EntityState<Mario>::Handler stateHandler) {
	state.SetHandler(stateHandler);

	if (stateHandler == &Mario::Fall) {
		onGround = false;
		dir.y = 1;
	}

	else if (stateHandler == &Mario::Jump) {
		velocity.y = (abs(velocity.x) == MAX_WALK_SPEED) ? -JUMP_SPEED_AFTER_MAX_WALK_SPEED : -JUMP_SPEED;
		dir.y = 1;
		onGround = false;
	}

	else if (stateHandler == &Mario::DieWait) {
		time = 0;
		SetAnimation("MarioDie");
		velocity = Vector2<float>(0, 0);
	}

	else if (stateHandler == &Mario::DieFall)
		velocity.y = -DEATH_JUMP_SPEED;
}

void Mario::Idle(float delta)
{
	velocity.y += Game::Gravity.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (velocity.x == 0)
		SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.idleLeft : animationSet.idleRight);
	else
		SetAnimation((lastPressedKeyHorizontal == DIK_LEFT) ? animationSet.walkLeft : animationSet.walkRight);

	if (velocity.x != 0)
		ApplyFriction(delta);

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
	velocity.y += Game::Gravity.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x != 0) {
		if (dir.x == Sign(velocity.x))
			SetAnimation((dir.x < 0) ? animationSet.walkLeft : animationSet.walkRight);
		else
			SetAnimation((dir.x < 0) ? animationSet.turnLeft : animationSet.turnRight);
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
	velocity.y += Game::Gravity.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x != 0) {
		if (dir.x == Sign(velocity.x))
			SetAnimation((dir.x < 0) ? animationSet.walkLeft : animationSet.walkRight);
		else
			SetAnimation((dir.x < 0) ? animationSet.turnLeft : animationSet.turnRight);
	}

	if (!Game::IsKeyDown(DIK_A)) {
		SwitchState(&Mario::Walk);
		return;
	}

	if (!onGround) {
		SwitchState(&Mario::Fall);
		return;
	}

	if (dir.x == 0) {
		SwitchState(&Mario::Idle);
		return;
	}

	if (onGround && Game::IsKeyPressed(DIK_S)) {
		SwitchState(&Mario::Jump);
		return;
	}
}

void Mario::Jump(float delta)
{
	ApplyHorizontalMovement(delta);
	velocity.y += ACCELERATION.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x == 0 && velocity.x != 0)
		ApplyFriction(delta);

	SetAnimation((dir.x < 0) ? animationSet.jumpLeft : animationSet.jumpRight);

	if (!Game::IsKeyDown(DIK_S) || velocity.y > 0) {
		velocity.y = max(velocity.y, -JUMP_SPEED_RELASE_EARLY);
		SwitchState(&Mario::Fall);
		return;
	}
}

void Mario::Fall(float delta) {
	ApplyHorizontalMovement(delta);
	velocity.y += ACCELERATION.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (dir.x == 0 && velocity.x != 0)
		ApplyFriction(delta);

	SetAnimation((dir.x < 0) ? animationSet.jumpLeft : animationSet.jumpRight);

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

void Mario::DieWait(float delta) {
	time += delta;

	if (time >= 0.75f)
		SwitchState(&Mario::DieFall);
}

void Mario::DieFall(float delta) {
	Vector2<float> acc(0, Game::Gravity.y / 1.5);
	Vector2<float> maxSpeed(0, MAX_FALL_SPEED);
	velocity += dir * acc * delta;
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