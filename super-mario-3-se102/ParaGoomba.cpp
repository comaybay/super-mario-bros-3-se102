#include "ParaGoomba.h"
#include "Goomba.h"
#include "CollisionHandling.h"
#include "Group.h"
#include "Scene.h"
#include "Mario.h"
#include "FXBoom.h"
#include "PointUpFactory.h"
#include "Contains.h"
#include "EntityUtils.h"
using namespace Entities;
using namespace Utils;

ParaGoomba::ParaGoomba(const std::string& colorType, const Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxGoomba", { "ParaGoombas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorType(colorType),
	onGround(false),
	colorCode(Color::ToColorCode(colorType)),
	wingLeft(Wing(this, WingDirection::LEFT, { -2, -10 })),
	wingRight(Wing(this, WingDirection::RIGHT, { 10, -10 }))
{
	SetAnimation(colorCode + "GoombaM");
	wingLeft.FlapDown();
	wingRight.FlapDown();
}

void ParaGoomba::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &ParaGoomba::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYERS });

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -Goomba::WALK_SPEED : Goomba::WALK_SPEED;
	}
	else
		velocity.x = -Goomba::WALK_SPEED;
}

void ParaGoomba::PostUpdate()
{
	Entity::PostUpdate();
	onGround = false;
}

void ParaGoomba::Render() {
	wingLeft.Render();
	wingRight.Render();
	Entity::Render();
}

void ParaGoomba::UpdateWings(float delta)
{
	wingLeft.Update(delta);
	wingRight.Update(delta);
}

void ParaGoomba::FlapUpWings() {
	wingLeft.FlapUp();
	wingRight.FlapUp();
}

void ParaGoomba::FlapDownWings() {
	wingLeft.FlapDown();
	wingRight.FlapDown();
}

void ParaGoomba::AutoFlapWings()
{
	wingLeft.AutoFlap();
	wingRight.AutoFlap();
}

void ParaGoomba::SetWingFlapSpeed(float speed)
{
	wingLeft.SetFlapSpeed(speed);
	wingRight.SetFlapSpeed(speed);
}

void ParaGoomba::OnCollision(CollisionData data)
{
	const EntityGroups& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups)) {
		LPMario player = static_cast<LPMario>(data.who);
		if (data.edge.y == 1) {
			player->Bounce();
			parentScene->AddEntity(new Entities::Goomba(colorType, position));
			parentScene->AddEntity(PointUpFactory::Create(position));
			parentScene->QueueFree(this);
			return;
		}

		if (!player->IsInvincible()) {
			player->TakeDamage();
			return;
		}
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y != 0)
			velocity.y = 0;

		if (data.edge.y == -1)
			onGround = true;

		else if (data.edge.x != 0)
			velocity.x = Goomba::WALK_SPEED * data.edge.x;

		return;
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, groups)) {
		if (data.edge.y == -1) {
			CollisionHandling::Slide(this, data);
			velocity.y = 0;
			onGround = true;
		}

		return;
	}
}

void ParaGoomba::GetKnockedOver(HDirection direction)
{
	Goomba* goomba = new Entities::Goomba(colorType, position);

	parentScene->AddEntity(goomba,
		[direction](LPEntity goomba) {
			static_cast<Goomba*>(goomba)->GetKnockedOver(direction);
		}
	);

	parentScene->QueueFree(this);

}