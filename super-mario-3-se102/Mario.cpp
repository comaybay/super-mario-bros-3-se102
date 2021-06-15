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
const Vector2<float> Mario::ACCELERATION = Vector2<float>(400, 560);
const float Mario::JUMP_SPEED = 270;
const float Mario::JUMP_SPEED_RELASE_EARLY = JUMP_SPEED / 1.75;
const float Mario::JUMP_SPEED_AFTER_MAX_WALK_SPEED = 290;
const float Mario::DEATH_JUMP_SPEED = JUMP_SPEED * 1.25;

Mario::Mario(Vector2<float> position) :
	Entity::Entity(position, "MarioSIR", Groups::PLAYER, GridType::NONE)
{
	state = &Mario::Idle;
	prevPressedKeyHorizontal = -1;
	dir = Vector2<float>(0, 1);
	time = 0;
	onGround = false;

	CollisionEngine::Subscribe(this, &Mario::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES });
}

void Mario::OnCollision(CollisionData data)
{
	std::vector<std::string> groups = data.who->GetEntityGroups();

	if (VectorHas(Groups::COLLISION_WALLS_TYPE_1, groups))
		WallSlide(data);

	else if (VectorHas(Groups::COLLISION_WALLS_TYPE_2, groups)) {
		if (data.edge.y == -1.0f)
			WallSlide(data);
	}

	else if (VectorHas(Groups::ENEMIES, groups)) {
		GetParentScene()->PlayerDeath();
		SwitchState(&Mario::DieWait);
	}
}

void Mario::WallSlide(CollisionData& data)
{
	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f) {
		onGround = true;
		velocity.y = 0;
	}

	if (data.edge.x != 0.0f)
		velocity.x = 0;
}

void Mario::UpdateHorizontalDirection()
{
	if (Game::IsKeyDown(DIK_RIGHT)) {
		prevPressedKeyHorizontal = DIK_RIGHT;
		dir.x = 1;
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		prevPressedKeyHorizontal = DIK_LEFT;
		dir.x = -1;
	}
	else {
		dir.x = 0;
	}
}

void Mario::Update(float delta)
{
	Entity::Update(delta);

	UpdateHorizontalDirection();
	auto prevState = state;
	(this->*state)(delta);

	//reset to check if still on ground
	onGround = false;
}

void Mario::SwitchState(void (Mario::* state)(float delta)) {
	this->state = state;

	if (state == &Mario::Fall)
		dir.y = 1;

	else if (state == &Mario::Jump) {
		velocity.y = (abs(velocity.x) == MAX_WALK_SPEED) ? -JUMP_SPEED_AFTER_MAX_WALK_SPEED : -JUMP_SPEED;
		dir.y = 1;
		onGround = false;
	}

	else if (state == &Mario::DieWait) {
		time = 0;
		SetAnimation("MarioDie");
		velocity = Vector2<float>(0, 0);
	}

	else if (state == &Mario::DieFall)
		velocity.y = -DEATH_JUMP_SPEED;
}

void Mario::Idle(float delta)
{
	if (velocity.x != 0)
		ApplyFriction(delta);
	else {
		if (prevPressedKeyHorizontal == DIK_RIGHT)
			SetAnimation("MarioSIR");
		else
			SetAnimation("MarioSIL");
	}

	if (Game::IsKeyPressed(DIK_S)) {
		SwitchState(&Mario::Jump);
		return;
	}

	if (dir.x != 0) {
		if (Game::IsKeyDown(DIK_A))
			SwitchState(&Mario::Run);
		else
			SwitchState(&Mario::Walk);
		return;
	}
}

void Mario::Walk(float delta)
{
	ApplyHorizontalMovement(delta);
	velocity.y += Game::Gravity.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (Game::IsKeyDown(DIK_A)) {
		SwitchState(&Mario::Run);
		return;
	}

	if (!onGround) {
		SwitchState(&Mario::Fall);
		return;
	}

	if (Game::IsKeyDown(DIK_RIGHT)) {
		SetAnimation("MarioSMR");
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		SetAnimation("MarioSML");
	}
	else {
		SwitchState(&Mario::Idle);
		return;
	}

	if (onGround && Game::IsKeyPressed(DIK_S)) {
		SwitchState(&Mario::Jump);
		return;
	}
}

void Mario::Run(float delta)
{
	ApplyHorizontalMovement(delta);
	velocity.y += Game::Gravity.y * delta;
	velocity.y = min(velocity.y, MAX_FALL_SPEED);

	if (!Game::IsKeyDown(DIK_A)) {
		SwitchState(&Mario::Walk);
		return;
	}

	if (!onGround) {
		SwitchState(&Mario::Fall);
		return;
	}

	if (Game::IsKeyDown(DIK_RIGHT)) {
		SetAnimation("MarioSMR");
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		SetAnimation("MarioSML");
	}
	else {
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

	if (dir.x > 0)
		SetAnimation("MarioSMR");
	else
		SetAnimation("MarioSML");

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

	if (Game::IsKeyDown(DIK_RIGHT))
		SetAnimation("MarioSMR");
	else if (Game::IsKeyDown(DIK_LEFT))
		SetAnimation("MarioSML");

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