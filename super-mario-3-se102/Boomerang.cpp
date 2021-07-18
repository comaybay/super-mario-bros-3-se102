#include "Boomerang.h"
#include "Group.h"
#include "Mario.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

const float Boomerang::MOVE_SPEED = 120;
const float Boomerang::ACCELERATION = 400;
const float Boomerang::UP_SPEED = 90;
const float Boomerang::OFFSET_END_Y = 8;
const float Boomerang::FALL_SPEED = 140;
const float Boomerang::DISTANCE_BEFORE_TURN_BACK = Constants::TILE_SIZE * 6.5f;

Boomerang::Boomerang(LPEntity holder, const Utils::Vector2<float>& offset, HDirection direction)
	: Entity(
		holder->GetPosition(),
		direction == HDirection::LEFT ? "BoomerangL" : "BoomerangR",
		"HitboxBoomerang",
		{ "Boomerangs", Group::ENEMIES },
		GridType::NONE),
	holder(holder),
	offset(offset),
	state(this, &Boomerang::Idle),
	dir(direction),
	endY(position.y + OFFSET_END_Y),
	startX(position.x),
	thrown(false)
{
	SetDetectable(false);
}

void Boomerang::OnReady()
{
	CollisionEngine::Subscribe(this, &Boomerang::OnCollision, { Group::PLAYERS , "BoomerangBrothers" });
}

void Boomerang::OnCollision(CollisionData data) {
	const EntityGroups& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups)) {
		LPMario player = static_cast<LPMario>(data.who);
		if (!player->IsInvincible())
			player->TakeDamage();

		return;
	}

	if (state.GetState() == &Boomerang::TurnBack && Contains("BoomerangBrothers", groups))
		parentScene->QueueFree(this);
}


void Boomerang::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void Boomerang::Idle(float delta)
{
	position = holder->GetPosition() + offset;
}

void Boomerang::MoveForward(float delta)
{
	velocity.y += FALL_SPEED * delta;
	velocity.y = min(velocity.y, FALL_SPEED);
	position.y = min(position.y, endY);

	if (abs(position.x - startX) >= DISTANCE_BEFORE_TURN_BACK) {
		SetDirection(dir == HDirection::LEFT ? HDirection::RIGHT : HDirection::LEFT);
		state.SetState(&Boomerang::TurnBack);
	}
}

void Boomerang::TurnBack(float delta)
{
	velocity.x += (dir == HDirection::LEFT ? -ACCELERATION : ACCELERATION) * delta;
	velocity.x = Clip(velocity.x, -MOVE_SPEED, MOVE_SPEED);

	velocity.y += FALL_SPEED * delta;
	velocity.y = min(velocity.y, FALL_SPEED / 2);
	position.y = min(position.y, endY);
}

void Boomerang::StartMoving()
{
	SetDetectable(true);

	velocity = {
		dir == HDirection::LEFT ? -MOVE_SPEED : MOVE_SPEED,
		-UP_SPEED
	};

	endY = position.y + OFFSET_END_Y;
	thrown = true;
	animation->SetAnimationSpeed(1);
	state.SetState(&Boomerang::MoveForward);
}

void Boomerang::SetDirection(HDirection direction)
{
	dir = direction;
	SetAnimation(direction == HDirection::LEFT ? "BoomerangL" : "BoomerangR", thrown ? 1.0f : 0);
}

void Boomerang::SetOffset(const Utils::Vector2<float> offset)
{
	this->offset = offset;
}

