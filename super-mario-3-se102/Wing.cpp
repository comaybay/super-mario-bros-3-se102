#include "Wing.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

Wing::Wing(LPEntity whoToFollow, Direction dir, const Vector2<float>& offset)
	: Entity::Entity(whoToFollow->GetPosition() + offset, AnimationId::NONE, HitboxId::NONE, "Wings", GridType::MOVABLE_ENTITIES),
	target(whoToFollow),
	offset(offset),
	dirCode((dir == Direction::LEFT) ? "L" : "R")
{
	SetAnimation("WingFlap" + dirCode);
}

void Wing::AutoFlap()
{
	SetAnimation("WingFlap" + dirCode);
}

void Wing::SetFlapSpeed(float speed)
{
	animation->SetAnimationSpeed(speed);
}

void Wing::FlapUp() {
	SetAnimation("WingDown" + dirCode);
}

void Wing::FlapDown() {
	SetAnimation("WingDown" + dirCode);
}

void Wing::Destroy() {
	parentScene->QueueFree(this);
}

void Wing::Update(float delta)
{
	animation->Update(delta);
	position = target->GetPosition() + offset;
}
