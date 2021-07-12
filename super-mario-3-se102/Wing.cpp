#include "Wing.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

Wing::Wing(LPEntity whoToFollow)
	: Entity::Entity(whoToFollow->GetPosition(), AnimationId::NONE, HitboxId::NONE, "Wings", GridType::MOVABLE_ENTITIES),
	target(whoToFollow),
	offset({ 0, 0 }),
	animSpeed(1)
{
	SetDirection(WingDirection::LEFT);
}

Wing::Wing(LPEntity whoToFollow, WingDirection dir, const Vector2<int>& offset)
	: Entity::Entity(whoToFollow->GetPosition() + offset, AnimationId::NONE, HitboxId::NONE, "Wings", GridType::MOVABLE_ENTITIES),
	target(whoToFollow),
	offset(offset)
{
	SetDirection(dir);
}

void Wing::AutoFlap()
{
	SetAnimation("WingFlap" + dirCode);
}

void Wing::SetDirection(WingDirection dir)
{
	dirCode = (dir == WingDirection::LEFT) ? "L" : "R";
	SetAnimation("WingFlap" + dirCode, animSpeed);
}

void Wing::SetOffset(const Vector2<float>& offset)
{
	position -= this->offset;
	this->offset = offset;
	position += this->offset;
}

void Wing::SetFlapSpeed(float speed)
{
	animSpeed = speed;
	animation->SetAnimationSpeed(animSpeed);
}

void Wing::FlapUp() {
	SetAnimation("WingDown" + dirCode, animSpeed);
}

void Wing::FlapDown() {
	SetAnimation("WingDown" + dirCode, animSpeed);
}

void Wing::Destroy() {
	parentScene->QueueFree(this);
}

void Wing::Update(float delta)
{
	Entity::Update(delta);
	position = target->GetPosition() + offset;
}
