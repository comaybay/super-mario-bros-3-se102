#include "NoteBlock.h"
#include "Group.h"
#include "Scene.h"
#include "Koopa.h"
#include "ContentFactory.h"
#include "Constants.h"
#include "Game.h"

using namespace Entities;
using namespace Utils;

NoteBlock::NoteBlock(std::string contentId, const Vector2<float>& position)
	: Entity::Entity(
		position,
		"NoteBlock",
		HitboxId::TILE_SIZE_HITBOX, { "NoteBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	state(EntityState<NoteBlock>(this, &NoteBlock::Idle)),
	blockHitMovementUp(MovementBlockHit(this, Direction::UP, position)),
	blockHitMovementDown(MovementBlockHit(this, Direction::DOWN, position)),
	contentId(contentId),
	targetPlayer(nullptr),
	pressedJump(false)
{
}

void NoteBlock::OnReady()
{
	CollisionEngine::Subscribe(this, &NoteBlock::OnCollision, { Group::PLAYERS, "Koopas" });
}

void NoteBlock::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void NoteBlock::Idle(float delta) {
}

void NoteBlock::StompedOn(float delta) {
	blockHitMovementDown.Update(delta);

	if (Game::IsKeyPressed(DIK_S))
		pressedJump = true;

	if (blockHitMovementDown.Finished()) {
		state.SetState(&NoteBlock::Idle);
		blockHitMovementDown.Reset();

		const Vector2<float>& vel = targetPlayer->GetVelocity();
		Vector2<float> newVel = { vel.x, pressedJump ? -350.0f : -200.0f };
		targetPlayer->SetVelocity(newVel);
		targetPlayer->SetOnNoteBlock(false);

		pressedJump = false;
		targetPlayer = nullptr;
	}
}

void NoteBlock::Hit(float delta) {
	blockHitMovementUp.Update(delta);

	if (blockHitMovementUp.Finished()) {
		state.SetState(&NoteBlock::Idle);
		blockHitMovementUp.Reset();
	}
}

void NoteBlock::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();
	if (Contains(Group::PLAYERS, groups) && data.edge.y != 0) {
		LPMario player = static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS));
		ExposeContent(player);

		if (data.edge.y == 1.0f) {
			targetPlayer = player;
			targetPlayer->SetOnNoteBlock(true);
			state.SetState(&NoteBlock::StompedOn);
		}
		else
			state.SetState(&NoteBlock::Hit);

		return;
	}

	if (Contains(std::string("Koopas"), groups)) {
		Koopa* koopa = static_cast<Koopa*>(data.who);

		if (koopa->IsSliding() && !parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
			state.SetState(&NoteBlock::Hit);
			LPMario player = static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS));
			ExposeContent(player);
		}

		return;
	}
}

void NoteBlock::ExposeContent(LPMario player)
{
	if (contentId != ContentId::NONE) {
		Vector2<float> contentPos = { position.x, position.y - Constants::TILE_SIZE };
		LPEntity content = ContentFactory(player).Create(contentId, contentPos);
		parentScene->AddEntity(content);
		contentId = ContentId::NONE;
	}
}
