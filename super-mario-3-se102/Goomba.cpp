#include "Entities.h"
#include "CollisionHandling.h"
#include "Game.h"
#include "Groups.h"
using namespace Entities;

float Goomba::maxFallSpeed = 230;

Goomba::Goomba(std::string colorCode, Vector2<float> position)
	: Entity(position, colorCode + "GoombaM", { "Goombas", Groups::ENEMIES }, GridType::MOVABLE_ENTITIES),
	speed(Vector2<float>(30, 0)),
	state(EntityState<Goomba>(this, &Goomba::MoveAround))
{
}

void Goomba::OnReady()
{
	CollisionEngine::Subscribe(this, &Goomba::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES, Groups::PLAYER });
	LPEntity player = GetParentScene()->GetEntitiesByGroup(Groups::PLAYER).front();
	velocity = (player->GetPosition().x < position.x) ? Vector2<float>(-30, 0) : Vector2<float>(30, 0);
}

void Goomba::MoveAround(float delta) {
	velocity += Game::Gravity * delta;
	velocity.y = min(velocity.y, maxFallSpeed);
}
void Goomba::Die(float delta) {}

void Goomba::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);;
}

void Goomba::OnCollision(CollisionData data)
{
	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f)
		velocity.y = 0;

	else if (data.edge.x != 0.0f)
		velocity.x = speed.x * -Sign(velocity.x);
}
