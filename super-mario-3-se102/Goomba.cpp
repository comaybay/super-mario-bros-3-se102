#include "Goomba.h"
#include "CollisionHandling.h"
#include "Groups.h"
#include "EntityConstants.h"
#include "Scene.h"
#include "Mario.h"
#include "BoomFX.h"
#include "PointUpFactory.h"
using namespace Entities;
using namespace Utils;

const float Goomba::WALK_SPEED = 30.0f;
const Vector2<float> Goomba::KNOCK_OVER_VELOCITY(60, -250);

Goomba::Goomba(std::string colorCode, Vector2<float> position)
	: Entity(position, colorCode + "GoombaM", "HitboxGoomba", { "Goombas", Groups::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorCode(colorCode),
	state(EntityState<Goomba>(this, &Goomba::StateMoveAround)),
	time(0)
{}

void Goomba::OnReady()
{
	CollisionEngine::Subscribe(this, &Goomba::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES, Groups::PLAYER });
	LPEntity player = parentScene->GetEntitiesByGroup(Groups::PLAYER).front();
	velocity.x = (player->GetPosition().x < position.x) ? -WALK_SPEED : WALK_SPEED;
}


void Goomba::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}

void Goomba::StateMoveAround(float delta) {

}

void Goomba::StompedOn(float delta) {
	time += delta;

	if (time >= 0.25f)
		parentScene->QueueFree(this);
}

void Goomba::StateKnockOver(float delta)
{
}

void Goomba::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (VectorHas(Groups::PLAYER, groups)) {
		Mario* mario = static_cast<Mario*>(data.who);

		if (data.edge.y == 1.0f) {
			mario->SwitchState(&Mario::Bounce);
			state.SetHandler(&Goomba::StompedOn);
			SetAnimation(colorCode + "GoombaDeath");
			velocity.x = 0;
			parentScene->AddEntity(PointUpFactory::Create(position));
			return;
		}

		mario->TakeDamage();
		velocity.x = (position.x < data.who->GetPosition().x) ? WALK_SPEED : -WALK_SPEED;
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
		velocity.x = WALK_SPEED * -Sign(velocity.x);

}

void Goomba::KnockOver(float horizontalDirection)
{
	SetEnabledForCollisionDetection(false);
	state.SetHandler(&Goomba::StateKnockOver);
	SetAnimation(colorCode + "GoombaKO");
	velocity = KNOCK_OVER_VELOCITY;
	velocity.x *= horizontalDirection;

	parentScene->AddEntity(new BoomFX(position));
	parentScene->AddEntity(PointUpFactory::Create(position));
}