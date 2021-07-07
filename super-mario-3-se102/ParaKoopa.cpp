#include "ParaKoopa.h"
#include "Koopa.h"
#include "Color.h"
#include "Mario.h"
#include "Group.h"
#include "Game.h"
#include "Contains.h"
#include "CollisionHandling.h"
#include "EntityConstants.h"
#include "PointUpFactory.h"
using namespace Entities;
using namespace Utils;

const float ParaKoopa::JUMP_SPEED = 280;

ParaKoopa::ParaKoopa(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxKoopa", { "Koopas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorType(colorType),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<ParaKoopa>(this, &ParaKoopa::JumpAround))
{}

ParaKoopa::~ParaKoopa()
{
	delete wing;

	if (state.GetHandler() == &ParaKoopa::StompedOn)
		parentScene->AddEntity(new Koopa(colorType, position));
}

void ParaKoopa::OnReady()
{
	Entity::OnReady();
	wing = new Wing(this);
	CollisionEngine::Subscribe(this, &ParaKoopa::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYER });
	const std::list<LPEntity>& playerGroup = parentScene->GetEntitiesByGroup(Group::PLAYER);

	if (playerGroup.empty()) {
		velocity.x = -Koopa::WALK_SPEED;
		SetAnimation(colorCode + "KoopaML");
		SetWingDirection(Wing::Direction::RIGHT);
		return;
	}

	LPEntity player = parentScene->GetEntitiesByGroup(Group::PLAYER).front();
	velocity.x = (player->GetPosition().x < position.x) ? -Koopa::WALK_SPEED : Koopa::WALK_SPEED;
	SetAnimation(colorCode + ((velocity.x < 0) ? "KoopaML" : "KoopaMR"));

	Wing::Direction wingDir = (player->GetPosition().x < position.x) ? Wing::Direction::RIGHT : Wing::Direction::LEFT;
	SetWingDirection(wingDir);
}


void ParaKoopa::SetWingDirection(Wing::Direction dir)
{
	wing->SetDirection(dir);

	if (dir == Wing::Direction::RIGHT)
		wing->SetOffset({ 8, 2 });
	else
		wing->SetOffset({ 0, 3 });
}

void ParaKoopa::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	wing->Update(delta);
}

void ParaKoopa::Render()
{
	Entity::Render();
	wing->Render();
}

void ParaKoopa::JumpAround(float delta) {
}

void ParaKoopa::StompedOn(float delta) {
}

void ParaKoopa::SwitchState(EntityState<ParaKoopa>::Handler handler)
{
	state.SetHandler(handler);
}

void ParaKoopa::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS, groups)) {
		HandleWallCollision(data);
		return;
	}

	if (Contains(Group::PLAYER, groups)) {
		HandlePlayerCollision(data);
		return;
	}

	if (Contains(Group::ENEMIES, groups)) {
		CollisionHandling::Slide(this, data);
		if (data.edge.x == 0.0f)
			return;

		velocity.x = Koopa::WALK_SPEED * data.edge.x;

		SetWingDirection((data.edge.x < 0) ? Wing::Direction::RIGHT : Wing::Direction::LEFT);
		std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
		SetAnimation(colorCode + anim);
		return;
	}
}

void ParaKoopa::HandlePlayerCollision(const CollisionData& data)
{
	LPMario mario = static_cast<LPMario>(data.who);
	if (data.edge.y == 1.0f) {
		mario->Bounce();
		SwitchState(&ParaKoopa::StompedOn);
		parentScene->AddEntity(PointUpFactory::Create(position));
		parentScene->QueueFree(this);
	}
	else {
		mario->TakeDamage();
		velocity.x = (mario->GetPosition().x < position.x) ? -Koopa::WALK_SPEED : Koopa::WALK_SPEED;

		SetWingDirection((velocity.x < 0) ? Wing::Direction::RIGHT : Wing::Direction::LEFT);
		std::string anim = (velocity.x < 0) ? "KoopaML" : "KoopaMR";
		SetAnimation(colorCode + anim);
	}
}

void ParaKoopa::HandleWallCollision(const CollisionData& data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y == -1.0f)
			velocity.y = -JUMP_SPEED;

		else if (data.edge.y == 1.0f)
			velocity.y = 0;

		else if (data.edge.x != 0.0f) {
			velocity.x = Koopa::WALK_SPEED * data.edge.x;

			std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
			SetWingDirection((data.edge.x < 0) ? Wing::Direction::RIGHT : Wing::Direction::LEFT);
			SetAnimation(colorCode + anim);
		}
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1.0f) {
		CollisionHandling::Slide(this, data);
		velocity.y = -JUMP_SPEED;
	}
}


