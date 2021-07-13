#include "Goomba.h"
#include "Color.h"
#include "CollisionHandling.h"
#include "Group.h"
#include "EntityConstants.h"
#include "Scene.h"
#include "Mario.h"
#include "FXBoom.h"
#include "PointUpFactory.h"
#include "Contains.h"
#include "EntityUtils.h"
using namespace Entities;
using namespace Utils;

const float Goomba::WALK_SPEED = 30.0f;

Goomba::Goomba(std::string colorType, Vector2<float> position)
	: Entity(position, AnimationId::NONE, "HitboxGoomba", { "Goombas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<Goomba>(this, &Goomba::MoveAround)),
	knockOverMovement(nullptr),
	time(0)
{
	SetAnimation(colorCode + "GoombaM");
}

void Goomba::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Goomba::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYERS });

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity player = parentScene->GetEntitiesByGroup(Group::PLAYERS).front();
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -WALK_SPEED : WALK_SPEED;
	}
	else
		velocity.x = -WALK_SPEED;
}

void Goomba::Update(float delta)
{
	Entity::Update(delta);

	state.Update(delta);

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}

void Goomba::MoveAround(float delta) {

}

void Goomba::StompedOn(float delta) {
	time += delta;

	if (time >= 0.25f)
		parentScene->QueueFree(this);
}

void Goomba::KnockedOver(float delta)
{
	knockOverMovement->Update(delta);
	if (knockOverMovement->Finished())
		parentScene->QueueFree(this);
}

void Goomba::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups)) {
		LPMario player = static_cast<LPMario>(data.who);

		if (data.edge.y == 1.0f) {
			player->Bounce();
			state.SetState(&Goomba::StompedOn);
			SetAnimation(colorCode + "GoombaDeath");
			velocity.x = 0;
			parentScene->AddEntity(PointUpFactory::Create(position));
			return;
		}

		if (!player->IsInvincible()) {
			player->TakeDamage();
			velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -WALK_SPEED : WALK_SPEED;
			return;
		}

		return;
	}

	if (Contains(Group::COLLISION_WALLS, groups)) {
		if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
			CollisionHandling::Slide(this, data);

			if (data.edge.y != 0.0f)
				velocity.y = 0;

			else if (data.edge.x != 0.0f)
				velocity.x = WALK_SPEED * data.edge.x;

			return;
		}

		if (data.edge.y == -1.0f)
			CollisionHandling::Slide(this, data);
	}
}

void Goomba::GetKnockedOver(HDirection direction)
{
	EnableForCollisionDetection(false);
	state.SetState(&Goomba::KnockedOver);
	SetAnimation(colorCode + "GoombaKO");
	knockOverMovement = new MovementKnockOver(this, direction);

	parentScene->AddEntity(new FXBoom(position));
	parentScene->AddEntity(PointUpFactory::Create(position));
}