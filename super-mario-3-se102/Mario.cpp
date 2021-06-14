#include "Entities.h"
#include "Game.h"
#include "EventHandler.h"
#include "CollisionHandling.h"
#include "Groups.h"
using namespace Entities;
using namespace Utils;

const Vector2<float> Mario::MAX_SPEED = Vector2<float>(100, 230);
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
	deathWaitDuration = 0;
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

void Entities::Mario::ClipVelocity()
{
	velocity.x = Clip(velocity.x, -MAX_SPEED.x, MAX_SPEED.x);
	velocity.y = min(velocity.y, MAX_SPEED.y);
}

void Mario::Update(float delta)
{
	Entity::Update(delta);

	auto prevState = state;
	(this->*state)(delta);

	//reset to check if still on ground
	onGround = false;
}

void Mario::SwitchState(void (Mario::* state)(float delta)) {
	this->state = state;

	if (state == &Mario::Fall) {
		dir.y = 1;
	}

	else if (state == &Mario::Jump) {
		velocity.y = (abs(velocity.x) == MAX_SPEED.x) ? -JUMP_SPEED_AFTER_MAX_WALK_SPEED : -JUMP_SPEED;
		dir.y = -1;
		onGround = false;
	}

	else if (state == &Mario::DieWait) {
		SetAnimation("MarioDie");
		velocity = Vector2<float>(0, 0);
	}

	else if (state == &Mario::DieFall)
		velocity.y = -DEATH_JUMP_SPEED;
}

void Mario::Idle(float delta)
{
	velocity += Game::Gravity * delta;
	ClipVelocity();

	dir.x = -Utils::Sign(velocity.x);
	if (abs(velocity.x) > ACCELERATION.x * delta)
		velocity.x += ACCELERATION.x * dir.x * delta;
	else {
		velocity.x = 0;
		if (prevPressedKeyHorizontal == DIK_LEFT) SetAnimation("MarioSIL");
		else if (prevPressedKeyHorizontal == DIK_RIGHT) SetAnimation("MarioSIR");
	}

	if (Game::IsKeyDown(DIK_RIGHT) || Game::IsKeyDown(DIK_LEFT)) {
		SwitchState(&Mario::Walk);
		return;
	}
	else if (onGround && Game::IsKeyPressed(DIK_S)) {
		SwitchState(&Mario::Jump);
		return;
	}
}

void Mario::Walk(float delta)
{
	velocity += Game::Gravity * delta;
	velocity.x += ACCELERATION.x * dir.x * delta;
	ClipVelocity();

	if (!onGround) {
		SwitchState(&Mario::Fall);
		return;
	}

	if (Game::IsKeyDown(DIK_RIGHT)) {
		prevPressedKeyHorizontal = DIK_RIGHT;
		dir.x = 1;
		SetAnimation("MarioSMR");
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		prevPressedKeyHorizontal = DIK_LEFT;
		SetAnimation("MarioSML");
		dir.x = -1;
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
	//TODO: IMPLEMENT THIS
}

void Mario::Jump(float delta)
{
	velocity.y += ACCELERATION.y * delta;
	velocity.x += ACCELERATION.x * dir.x * delta;
	ClipVelocity();

	if (!Game::IsKeyDown(DIK_S) || velocity.y > 0) {
		velocity.y = max(velocity.y, -JUMP_SPEED_RELASE_EARLY);
		SwitchState(&Mario::Fall);
		return;
	}

	if (Game::IsKeyDown(DIK_RIGHT)) {
		prevPressedKeyHorizontal = DIK_RIGHT;
		dir.x = 1;
		SetAnimation("MarioSMR");
	}

	else if (Game::IsKeyDown(DIK_LEFT)) {
		prevPressedKeyHorizontal = DIK_LEFT;
		SetAnimation("MarioSML");
		dir.x = -1;
	}

	else {
		dir.x = -Utils::Sign(velocity.x);

		if (abs(velocity.x) > ACCELERATION.x * delta)
			velocity.x += ACCELERATION.x * dir.x * delta;
		else {
			velocity.x = 0;
			if (prevPressedKeyHorizontal == DIK_LEFT) SetAnimation("MarioSIL");
			else if (prevPressedKeyHorizontal == DIK_RIGHT) SetAnimation("MarioSIR");
		}
		return;
	}


}

void Mario::Fall(float delta) {

	velocity.x += ACCELERATION.x * dir.x * delta;
	ClipVelocity();

	if (onGround) {
		SwitchState(&Mario::Idle);
		return;
	}
	velocity += Game::Gravity * delta;
	velocity.y = min(velocity.y, MAX_SPEED.y);

	if (Game::IsKeyDown(DIK_RIGHT)) {
		prevPressedKeyHorizontal = DIK_RIGHT;
		dir.x = 1;
		SetAnimation("MarioSMR");
	}
	else if (Game::IsKeyDown(DIK_LEFT)) {
		prevPressedKeyHorizontal = DIK_LEFT;
		SetAnimation("MarioSML");
		dir.x = -1;
	}
	else {
		dir.x = -Utils::Sign(velocity.x);

		if (abs(velocity.x) > ACCELERATION.x * delta)
			velocity.x += ACCELERATION.x * dir.x * delta;
		else {
			velocity.x = 0;
			if (prevPressedKeyHorizontal == DIK_LEFT) SetAnimation("MarioSIL");
			else if (prevPressedKeyHorizontal == DIK_RIGHT) SetAnimation("MarioSIR");
		}
		return;
	}

}

void Mario::DieWait(float delta) {
	deathWaitDuration += delta;

	if (deathWaitDuration >= 0.75f)
		SwitchState(&Mario::DieFall);
}

void Mario::DieFall(float delta) {
	velocity.y += Game::Gravity.y / 1.5 * delta;
	ClipVelocity();
}
