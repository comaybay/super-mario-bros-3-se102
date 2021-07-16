#include "SuperNoteBlock.h"
#include "Group.h"
#include "Scene.h"
#include "Koopa.h"
#include "ContentFactory.h"
#include "Constants.h"
#include "Game.h"

using namespace Entities;
using namespace Utils;

SuperNoteBlock::SuperNoteBlock(const Vector2<float>& position)
	: Entity::Entity(
		position,
		AnimationId::NONE,
		HitboxId::TILE_SIZE_HITBOX, { "SuperNoteBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	state(EntityState<SuperNoteBlock>(this, &SuperNoteBlock::Idle)),
	blockHitMovementUp(MovementBlockHit(this, Direction::UP, position)),
	blockHitMovementDown(MovementBlockHit(this, Direction::DOWN, position)),
	targetPlayer(nullptr),
	pressedJump(false)
{
}

void SuperNoteBlock::OnReady()
{
	SetDetectable(false);
	CollisionEngine::Subscribe(this, &SuperNoteBlock::OnCollision, { Group::PLAYERS, "Koopas" });
}

void SuperNoteBlock::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void SuperNoteBlock::Idle(float delta) {
}

void SuperNoteBlock::StompedOn(float delta) {
	blockHitMovementDown.Update(delta);

	if (Game::IsKeyPressed(DIK_S))
		pressedJump = true;

	if (blockHitMovementDown.Finished()) {
		state.SetState(&SuperNoteBlock::Idle);
		blockHitMovementDown.Reset();

		//TODO: implement super note block behaviour
		const Vector2<float>& vel = targetPlayer->GetVelocity();
		Vector2<float> newVel = { vel.x, pressedJump ? -1000.0f : -200.0f };
		targetPlayer->SetVelocity(newVel);
		targetPlayer->SetOnNoteBlock(false);

		pressedJump = false;
		targetPlayer = nullptr;
	}
}

void SuperNoteBlock::Hit(float delta) {
	blockHitMovementUp.Update(delta);

	if (blockHitMovementUp.Finished()) {
		state.SetState(&SuperNoteBlock::Idle);
		blockHitMovementUp.Reset();
	}
}

void SuperNoteBlock::OnCollision(CollisionData data)
{
	const EntityGroups& groups = data.who->GetEntityGroups();
	if (Contains(Group::PLAYERS, groups) && data.edge.y != 0) {
		LPMario player = static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS));

		if (data.edge.y == 1 && IsDetectable()) {
			targetPlayer = player;
			state.SetState(&SuperNoteBlock::StompedOn);
			return;
		}

		if (data.edge.y == -1) {
			SetDetectable(true);
			SetAnimation("SuperNoteBlock");
			state.SetState(&SuperNoteBlock::Hit);
			return;
		}

		return;
	}

	if (Contains(std::string("Koopas"), groups)) {
		Koopa* koopa = static_cast<Koopa*>(data.who);

		if (koopa->IsSliding())
			state.SetState(&SuperNoteBlock::Hit);

		return;
	}
}

