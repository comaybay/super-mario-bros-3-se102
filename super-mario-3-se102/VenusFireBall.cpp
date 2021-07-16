#include "VenusFireBall.h"
#include "Group.h"
#include "Scene.h"
#include "Mario.h"
using namespace Entities;
using namespace Utils;

const float VenusFireBall::SPEED = 60;

VenusFireBall::VenusFireBall(const Utils::Vector2<float>& position, float angle)
	: Entity(position, "FireBall", "HitboxFireBall", { "FireBalls", Group::ENEMIES }, GridType::NONE)
{
	velocity = Vector2<float>{ SPEED, 0 }.RotateTo(angle);
}

void VenusFireBall::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &VenusFireBall::OnCollision, { Group::PLAYERS });
}

void VenusFireBall::OnCollision(CollisionData data)
{
	LPMario player = static_cast<LPMario>(data.who);

	if (!player->IsInvincible()) {
		player->TakeDamage();
		parentScene->QueueFree(this);
	}
}
