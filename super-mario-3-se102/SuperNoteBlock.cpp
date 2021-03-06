#include "SuperNoteBlock.h"
#include "Group.h"
#include "Scene.h"
#include "Koopa.h"
#include "ContentFactory.h"
#include "Constants.h"
#include "Game.h"
#include "PlayerVariables.h"

using namespace Entities;
using namespace Utils;

const float SuperNoteBlock::WAIT_DURATION = 1;

SuperNoteBlock::SuperNoteBlock(const std::string& scenePath, const Vector2<float>& position)
	: Entity::Entity(
		position,
		AnimationId::NONE,
		HitboxId::TILE_SIZE_HITBOX, { "SuperNoteBlocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	scenePath(scenePath),
	state(EntityState<SuperNoteBlock>(this, &SuperNoteBlock::Idle)),
	blockHitMovementUp(MovementBlockHit(this, Direction::UP, position)),
	blockHitMovementDown(MovementBlockHit(this, Direction::DOWN, position)),
	targetPlayer(nullptr),
	pressedJump(false),
	time(0)
{
}

void SuperNoteBlock::OnReady()
{
	Entity::OnReady();
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
		blockHitMovementDown.Reset();

		const Vector2<float>& vel = targetPlayer->GetVelocity();
		Vector2<float> newVel = { vel.x, pressedJump ? -1000.0f : -200.0f };
		targetPlayer->SetVelocity(newVel);
		targetPlayer->SetOnNoteBlock(false);

		state.SetState(pressedJump ? &SuperNoteBlock::PrepareSwitchScene : &SuperNoteBlock::Idle);
		if (pressedJump)
			parentScene->GetCamera().StopFollowingEntity();

		pressedJump = false;
		targetPlayer = nullptr;
	}
}

void SuperNoteBlock::PrepareSwitchScene(float delta)
{
	time += delta;
	if (time >= WAIT_DURATION) {
		PlayerVariables::SetIsFlownBySuperNoteBlock(true);
		Game::QueueFreeAndSwitchScene(scenePath);
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
		LPKoopa koopa = static_cast<LPKoopa>(data.who);

		if (koopa->IsSliding())
			state.SetState(&SuperNoteBlock::Hit);

		return;
	}
}

