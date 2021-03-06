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

Goomba::~Goomba()
{
	delete knockedOverMovement;
	knockedOverMovement = nullptr;
}

Goomba::Goomba(std::string colorType, Vector2<float> position)
	: Entity(position, AnimationId::NONE, "HitboxGoomba", { "Goombas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<Goomba>(this, &Goomba::MoveAround)),
	knockedOverMovement(nullptr),
	time(0)
{
	SetAnimation(colorCode + "GoombaM");
}

void Goomba::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Goomba::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYERS });

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -WALK_SPEED : WALK_SPEED;
	}
	else
		velocity.x = -WALK_SPEED;
}

void Goomba::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void Goomba::MoveAround(float delta)
{
	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);
}

void Goomba::StompedOn(float delta)
{
	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);

	time += delta;
	if (time >= 0.25f)
		parentScene->QueueFree(this);
}

void Goomba::KnockedOver(float delta)
{
	knockedOverMovement->Update(delta);

	if (knockedOverMovement->Finished())
		parentScene->QueueFree(this);
}

void Goomba::OnCollision(CollisionData data)
{
	const EntityGroups& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups)) {
		LPMario player = static_cast<LPMario>(data.who);

		if (data.edge.y == 1) {
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

	if (Contains(Group::ENEMIES, groups) && data.edge.x != 0) {
		velocity.x = WALK_SPEED * data.edge.x;
		return;
	}

	if (Contains(Group::COLLISION_WALLS, groups)) {
		if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
			CollisionHandling::Slide(this, data);

			if (data.edge.y != 0)
				velocity.y = 0;

			else if (data.edge.x != 0)
				velocity.x = WALK_SPEED * data.edge.x;

			return;
		}

		if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1) {
			CollisionHandling::Slide(this, data);
			return;
		}

		return;
	}
}

void Goomba::GetKnockedOver(HDirection direction)
{
	SetDetectable(false);
	CollisionEngine::Unsubscribe(this, &Goomba::OnCollision);
	SetAnimation(colorCode + "GoombaKO");

	knockedOverMovement = new MovementKnockedOver(this, direction);
	state.SetState(&Goomba::KnockedOver);

	parentScene->AddEntity(new FXBoom(position));
	parentScene->AddEntity(PointUpFactory::Create(position));
}