#include "Brick.h"
#include "FXBrickBreak.h"
#include "EmptyBlock.h"
#include "Koopa.h"
#include "Group.h"
#include "Scene.h"
#include "Mario.h"
#include "ContentFactory.h"
#include "FXSmoke.h"
using namespace Entities;
using namespace Utils;

Brick::Brick(const std::string& contentId, const Utils::Vector2<float>& position)
	: Entity::Entity(
		position,
		"BrickNormal",
		HitboxId::TILE_SIZE_HITBOX,
		{ "Bricks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	contentId(contentId),
	state(EntityState<Brick>(this, &Brick::Idle)),
	blockHitMovement(this, Direction::UP, position)
{
}

void Brick::OnReady() {
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Brick::OnCollision, { Group::PLAYERS, "Koopas" });
}

void Brick::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void Brick::Idle(float delta) {
}

void Brick::Hit(float delta) {
	blockHitMovement.Update(delta);

	if (blockHitMovement.Finished()) {
		state.SetState(&Brick::Idle);
		blockHitMovement.Reset();
	}
}

void Brick::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();
	if (Contains(Group::PLAYERS, groups) && data.edge.y == -1) {
		LPMario player = static_cast<LPMario>(data.who);
		if (player->GetPowerLevel() != PlayerPowerLevel::SMALL)
			ExposeContent(player);
		else
			state.SetState(&Brick::Hit);

		return;
	}

	if (Contains(std::string("Koopas"), groups) && !parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		Koopa* koopa = static_cast<Koopa*>(data.who);
		LPMario player = static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS));

		if (data.edge.x != 0 && koopa->IsSliding())
			ExposeContent(player);

		return;
	}
}

void Brick::ExposeContent(LPMario player)
{
	if (contentId == ContentId::NONE)
	{
		parentScene->AddEntity(new FXBrickBreak(position));
		parentScene->QueueFree(this);
		return;
	}

	Vector2<float> contentPos = { position.x, position.y - Constants::TILE_SIZE };
	LPEntity content = ContentFactory(player).Create(contentId, contentPos);
	contentId = ContentId::NONE;

	parentScene->AddEntity(content);
	parentScene->AddEntity(new EmptyBlock(position));
	parentScene->AddEntity(new FXSmoke(contentPos));
	parentScene->QueueFree(this);
}
