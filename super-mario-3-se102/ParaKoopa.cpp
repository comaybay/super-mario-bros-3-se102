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
#include "EntityUtils.h"
using namespace Entities;
using namespace Utils;

const float ParaKoopa::JUMP_SPEED = 210;
const float ParaKoopa::FALL_SPEED = EntityConstants::GRAVITY / 1.5f;

ParaKoopa::ParaKoopa(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxKoopa", { "ParaKoopas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorType(colorType),
	colorCode(Color::ToColorCode(colorType)),
	wing(Wing(this)),
	state(EntityState<ParaKoopa>(this, &ParaKoopa::JumpAround))
{}

void ParaKoopa::OnReady()
{
	Entity::OnReady();
	wing.AutoFlap();
	CollisionEngine::Subscribe(this, &ParaKoopa::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYERS });

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -Koopa::WALK_SPEED : Koopa::WALK_SPEED;
		SetAnimation(colorCode + ((velocity.x < 0) ? "KoopaML" : "KoopaMR"));

		WingDirection wingDir = EntityUtils::IsOnLeftSideOf(this, player) ? WingDirection::RIGHT : WingDirection::LEFT;
		SetWingDirection(wingDir);
	}
	else {
		velocity.x = -Koopa::WALK_SPEED;
		SetAnimation(colorCode + "KoopaML");
		SetWingDirection(WingDirection::RIGHT);
	}
}


void ParaKoopa::SetWingDirection(WingDirection dir)
{
	static const Vector2<float> WING_OFFSET_RIGHT = { 8, 2 };
	static const Vector2<float> WING_OFFSET_LEFT = { 0, 3 };
	wing.SetDirection(dir);

	if (dir == WingDirection::RIGHT)
		wing.SetOffset(WING_OFFSET_RIGHT);
	else
		wing.SetOffset(WING_OFFSET_LEFT);
}

void ParaKoopa::Update(float delta)
{
	Entity::Update(delta);

	state.Update(delta);
	velocity.y += FALL_SPEED * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	wing.Update(delta);
}

void ParaKoopa::Render()
{
	Entity::Render();
	wing.Render();
}

void ParaKoopa::GetKnockedOver(HDirection direction) {
	Koopa* koopa = new Koopa(colorType, position);

	parentScene->AddEntity(koopa,
		[direction](LPEntity koopa) {
			static_cast<Koopa*>(koopa)->GetKnockedOver(direction);
		}
	);

	parentScene->QueueFree(this);
}

void ParaKoopa::JumpAround(float delta) {
}

void ParaKoopa::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS, groups)) {
		HandleWallCollision(data);
		return;
	}

	if (Contains(Group::PLAYERS, groups)) {
		HandlePlayerCollision(data);
		return;
	}

	if (Contains(Group::ENEMIES, groups)) {
		CollisionHandling::Slide(this, data);
		if (data.edge.x == 0)
			return;

		velocity.x = Koopa::WALK_SPEED * data.edge.x;

		SetWingDirection((data.edge.x < 0) ? WingDirection::RIGHT : WingDirection::LEFT);
		std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
		SetAnimation(colorCode + anim);
		return;
	}
}

void ParaKoopa::HandlePlayerCollision(const CollisionData& data)
{
	LPMario player = static_cast<LPMario>(data.who);
	if (data.edge.y == 1) {
		player->Bounce();
		parentScene->AddEntity(new Koopa(colorType, position));
		parentScene->AddEntity(PointUpFactory::Create(position));
		parentScene->QueueFree(this);
		return;
	}

	if (!player->IsInvincible()) {
		player->TakeDamage();
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -Koopa::WALK_SPEED : Koopa::WALK_SPEED;

		SetWingDirection((velocity.x < 0) ? WingDirection::RIGHT : WingDirection::LEFT);
		std::string anim = (velocity.x < 0) ? "KoopaML" : "KoopaMR";
		SetAnimation(colorCode + anim);
		return;
	}
}

void ParaKoopa::HandleWallCollision(const CollisionData& data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y == -1)
			velocity.y = -JUMP_SPEED;

		else if (data.edge.y == 1)
			velocity.y = 0;

		else if (data.edge.x != 0) {
			velocity.x = Koopa::WALK_SPEED * data.edge.x;

			std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
			SetWingDirection((data.edge.x < 0) ? WingDirection::RIGHT : WingDirection::LEFT);
			SetAnimation(colorCode + anim);
		}
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1) {
		CollisionHandling::Slide(this, data);
		velocity.y = -JUMP_SPEED;
	}
}


