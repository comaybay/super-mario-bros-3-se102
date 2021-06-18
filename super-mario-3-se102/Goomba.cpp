#include "Goomba.h"
#include "CollisionHandling.h"
#include "Groups.h"
#include "EntityConstants.h"
#include "Scene.h"
#include "Mario.h"
using namespace Entities;
using namespace Utils;


Goomba::Goomba(std::string colorCode, Vector2<float> position)
	: Entity(position, colorCode + "GoombaM", "HitboxGoomba", { "Goombas", Groups::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorCode(colorCode),
	speed(Vector2<float>(30, 0)),
	state(EntityState<Goomba>(this, &Goomba::MoveAround)),
	time(0)
{}

void Goomba::OnReady()
{
	CollisionEngine::Subscribe(this, &Goomba::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES, Groups::PLAYER });
	LPEntity player = parentScene->GetEntitiesByGroup(Groups::PLAYER).front();
	velocity = (player->GetPosition().x < position.x) ? Vector2<float>(-30, 0) : Vector2<float>(30, 0);
}

void Goomba::MoveAround(float delta) {
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}

void Goomba::Die(float delta) {
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	time += delta;

	if (time >= 0.25f)
		parentScene->QueueFree(this);
}

void Goomba::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);;
}

void Goomba::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (VectorHas(Groups::PLAYER, groups)) {
		Mario* mario = static_cast<Mario*>(data.who);

		if (data.edge.y == 1.0f) {
			mario->SwitchState(&Mario::Bounce);
			state.SetHandler(&Goomba::Die);
			SetAnimation(colorCode + "GoombaDeath");
			velocity.x = 0;
			return;
		}

		mario->TakeDamage();
		velocity.x = (position.x < data.who->GetPosition().x) ? speed.x : -speed.x;
		return;
	}

	if (VectorHas(Groups::COLLISION_WALLS_TYPE_2, groups)) {
		if (data.edge.y == -1.0f)
			CollisionHandling::Slide(this, data);

		return;
	}

	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f)
		velocity.y = 0;

	else if (data.edge.x != 0.0f)
		velocity.x = speed.x * -Sign(velocity.x);

}
