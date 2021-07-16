#include "ItemWoodBlock.h"
#include "Group.h"
#include "ContentFactory.h"
#include "Scene.h"
#include "Koopa.h"
#include "GhostBlock.h"

using namespace Entities;
using namespace Utils;

const float ItemWoodBlock::BOUNCE_BACK_SPEED = 90;

ItemWoodBlock::ItemWoodBlock(const std::string& contentId, const Vector2<float>& position)
	: Entity(
		position,
		"ItemWoodBlock",
		HitboxId::TILE_SIZE_HITBOX,
		{ "ItemWoodBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	contentId(contentId),
	state(EntityState<ItemWoodBlock>(this, &ItemWoodBlock::Idle)),
	blockHitMovement(nullptr),
	ghostBlock(new GhostBlock(position))
{
}

ItemWoodBlock::~ItemWoodBlock()
{
	delete blockHitMovement;
	parentScene->QueueFree(ghostBlock);
}

void ItemWoodBlock::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void ItemWoodBlock::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &ItemWoodBlock::OnCollision, { Group::PLAYERS, "Koopas" });
	parentScene->AddEntity(ghostBlock);
	ghostBlock->Activate(false);
}

void ItemWoodBlock::OnCollision(CollisionData data)
{
	if (state.GetState() == &ItemWoodBlock::Touch)
		return;

	const EntityGroups& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups) && data.edge.x != 0) {

		Vector2<float> playerVel = data.who->GetVelocity();
		playerVel.x = data.edge.x == -1 ? BOUNCE_BACK_SPEED : -BOUNCE_BACK_SPEED;
		data.who->SetVelocity(playerVel);

		delete blockHitMovement;
		Direction dir = data.edge.x == -1 ? Direction::LEFT : Direction::RIGHT;
		blockHitMovement = new MovementBlockHit(this, dir, position);
		SetAnimation("EmptyWoodBlock");

		ghostBlock->Activate(true);
		state.SetState(&ItemWoodBlock::Touch);

		if (contentId != ContentId::NONE)
			ExposeContent(static_cast<LPMario>(data.who));

		return;
	}

	if (Contains("Koopas", groups) && data.edge.x != 0) {
		Koopa* koopa = static_cast<Koopa*>(data.who);

		if (!koopa->IsSliding())
			return;

		delete blockHitMovement;

		blockHitMovement = new MovementBlockHit(this, Direction::UP, position);
		SetAnimation("EmptyWoodBlock");
		ghostBlock->Activate(true);
		state.SetState(&ItemWoodBlock::Touch);

		if (contentId != ContentId::NONE && !parentScene->IsEntityGroupEmpty(Group::PLAYERS))
			ExposeContent(static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS)));

		return;
	}
}

void ItemWoodBlock::ExposeContent(LPMario player)
{
	if (contentId == ContentId::NONE)
		return;

	LPEntity content = ContentFactory(player).Create(contentId, { position.x, position.y - Constants::TILE_SIZE });
	contentId = ContentId::NONE;
	parentScene->AddEntity(content);
}

void ItemWoodBlock::Idle(float delta)
{
}

void ItemWoodBlock::Touch(float delta)
{
	blockHitMovement->Update(delta);

	if (blockHitMovement->Finished()) {
		ghostBlock->Activate(false);
		SetAnimation("ItemWoodBlock");
		state.SetState(&ItemWoodBlock::Idle);
	}
}
