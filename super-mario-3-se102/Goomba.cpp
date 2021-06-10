#include "Entities.h"
#include "CollisionHandling.h"
#include "Game.h"
#include "Groups.h"
using namespace Entities;

float Goomba::maxFallSpeed = 230;

Goomba::Goomba(std::string colorCode, Vector2 position)
	: Entity(position, colorCode + "GoombaM", { "Goombas", Groups::ENEMIES }, GridType::MOVABLE_ENTITIES)
{
	velocity = Vector2(30, 0);
	speed = Vector2(30, 0);
	CollisionEngine::Subscribe(this, &Goomba::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES });
}

void Goomba::Update(float delta)
{
	Entity::Update(delta);

	velocity += Game::Gravity * delta;
	velocity.y = min(velocity.y, maxFallSpeed);
}

void Goomba::OnCollision(CollisionData data)
{
	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f)
		velocity.y = 0;

	else if (data.edge.x != 0.0f)
		velocity.x = speed.x * -Utils::Sign(velocity.x);
}
