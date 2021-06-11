#include "Entities.h"
#include "Game.h"
#include "EventHandler.h"
#include "CollisionHandling.h"
#include "Groups.h"
using namespace Entities;
using namespace Utils;

const Vector2<float> Mario::maxSpeed = Vector2<float>(100, 230);
const Vector2<float> Mario::acceleration = Vector2<float>(400, 560);
const float Mario::jumpSpeed = 270;
const float Mario::jumpSpeedAfterMaxWalkSpeed = 290;
const float Mario::jumpSpeedReleaseEarly = 270 / 1.75;

Mario::Mario(Vector2<float> position) :
	Entity::Entity(position, "MarioSIR", Groups::PLAYER, GridType::MOVABLE_ENTITIES)
{

	pStateUpdate = &Mario::IdleUpdate;
	prevPressedKeyHorizontal = -1;
	dir = Vector2<float>(0, 1);
	jumpDuration = 0;
	onGround = false;

	CollisionEngine::Subscribe(this, &Mario::OnCollision, { Groups::COLLISION_WALLS_TYPE_1, Groups::COLLISION_WALLS_TYPE_2 });
}

void Mario::OnCollision(CollisionData data)
{
	std::vector<std::string> groups = data.who->GetEntityGroups();

	if (Utils::VectorHas(Groups::COLLISION_WALLS_TYPE_1, groups))
		WallSide(data);

	else if (Utils::VectorHas(Groups::COLLISION_WALLS_TYPE_2, groups)) {
		if (data.edge.y == -1.0f)
			WallSide(data);
	}
}

void Mario::WallSide(CollisionData& data)
{
	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f) {
		onGround = true;
		velocity.y = 0;
	}

	if (data.edge.x != 0.0f)
		velocity.x = 0;
}

void Mario::Update(float delta)
{
	Entity::Update(delta);

	auto prevState = pStateUpdate;
	(this->*pStateUpdate)(delta);

	//reset to check if still on ground
	onGround = false;
}

void Mario::SwitchState(void (Mario::* state)(float delta)) {
	if (pStateUpdate == &Mario::JumpUpdate) {
		jumpDuration = 0;
	}

	pStateUpdate = state;

	if (pStateUpdate == &Mario::FallUpdate) {
		dir.y = 1;
	}

	if (pStateUpdate == &Mario::JumpUpdate) {
		velocity.y = (abs(velocity.x) == maxSpeed.x) ? -jumpSpeedAfterMaxWalkSpeed : -jumpSpeed;
		dir.y = -1;
		onGround = false;
	}
}

void Mario::IdleUpdate(float delta)
{
	velocity += Game::Gravity * delta;

	dir.x = -Utils::Sign(velocity.x);
	if (abs(velocity.x) > acceleration.x * delta)
		velocity.x += acceleration.x * dir.x * delta;
	else {
		velocity.x = 0;
		if (prevPressedKeyHorizontal == DIK_LEFT) SetAnimation("MarioSIL");
		else if (prevPressedKeyHorizontal == DIK_RIGHT) SetAnimation("MarioSIR");
	}

	if (Game::IsKeyDown(DIK_RIGHT) || Game::IsKeyDown(DIK_LEFT)) {
		SwitchState(&Mario::WalkUpdate);
		return;
	}
	else if (onGround && Game::IsKeyPressed(DIK_S)) {
		SwitchState(&Mario::JumpUpdate);
		return;
	}
}

void Mario::WalkUpdate(float delta)
{
	velocity += Game::Gravity * delta;
	velocity.x += acceleration.x * dir.x * delta;
	velocity.x = Clip(velocity.x, -maxSpeed.x, maxSpeed.x);

	if (!onGround) {
		SwitchState(&Mario::FallUpdate);
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
		SwitchState(&Mario::IdleUpdate);
		return;
	}

	if (onGround && Game::IsKeyPressed(DIK_S)) {
		SwitchState(&Mario::JumpUpdate);
		return;
	}
}

void Mario::RunUpdate(float delta)
{
	//TODO: IMPLEMENT THIS
}

void Mario::JumpUpdate(float delta)
{
	velocity.y += acceleration.y * delta;
	velocity.x += acceleration.x * dir.x * delta;
	velocity.x = Clip(velocity.x, -maxSpeed.x, maxSpeed.x);

	jumpDuration += delta;
	if (!Game::IsKeyDown(DIK_S) || velocity.y > 0) {
		velocity.y = max(velocity.y, -jumpSpeedReleaseEarly);
		SwitchState(&Mario::FallUpdate);
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

		if (abs(velocity.x) > acceleration.x * delta)
			velocity.x += acceleration.x * dir.x * delta;
		else {
			velocity.x = 0;
			if (prevPressedKeyHorizontal == DIK_LEFT) SetAnimation("MarioSIL");
			else if (prevPressedKeyHorizontal == DIK_RIGHT) SetAnimation("MarioSIR");
		}
		return;
	}


}

void Mario::FallUpdate(float delta) {

	velocity.x += acceleration.x * dir.x * delta;
	velocity.x = (dir.x < 0) ? max(velocity.x, -maxSpeed.x) : min(velocity.x, maxSpeed.x);

	if (onGround) {
		SwitchState(&Mario::IdleUpdate);
		return;
	}
	velocity += Game::Gravity * delta;
	velocity.y = min(velocity.y, maxSpeed.y);

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

		if (abs(velocity.x) > acceleration.x * delta)
			velocity.x += acceleration.x * dir.x * delta;
		else {
			velocity.x = 0;
			if (prevPressedKeyHorizontal == DIK_LEFT) SetAnimation("MarioSIL");
			else if (prevPressedKeyHorizontal == DIK_RIGHT) SetAnimation("MarioSIR");
		}
		return;
	}

}
