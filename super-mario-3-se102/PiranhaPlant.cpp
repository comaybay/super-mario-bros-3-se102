#pragma once
#include "PiranhaPlant.h"
#include "Group.h"
#include "Color.h"
#include "Scene.h"
#include "Contains.h"
#include "Mario.h"
#include "Koopa.h"
#include "FXBoom.h"
#include "PointUpFactory.h"

using namespace Entities;
using namespace Utils;

const float PiranhaPlant::WAIT_DURATION = 1;
const float PiranhaPlant::MOVE_SPEED = 60;
const float PiranhaPlant::SAFE_DISTANCE = Constants::TILE_SIZE * 2;

PiranhaPlant::PiranhaPlant(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(
		{ position.x,  position.y + Constants::TILE_SIZE * 2 },
		Color::ToColorCode(colorType) + "PiranhaPlant",
		"HitboxPiranhaPlant", { "PiranhaPlant", Group::ENEMIES }, GridType::MOVABLE_ENTITIES, true),
	targetPlayer(nullptr),
	stopYMoveUp(position.y),
	stopYMoveDown(position.y + Constants::TILE_SIZE * 2),
	time(0),
	state(EntityState<PiranhaPlant>(this, &PiranhaPlant::CheckDistance))
{
}

void PiranhaPlant::OnReady()
{
	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		targetPlayer = parentScene->GetEntityOfGroup(Group::PLAYERS);
		targetPlayer->GetDestroyEvent().Subscribe(this, &PiranhaPlant::OnPlayerDestroy);
	}
}

void PiranhaPlant::OnPlayerDestroy(LPEntity entity) {
	targetPlayer = nullptr;
}

void PiranhaPlant::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups)) {
		LPMario player = static_cast<LPMario>(data.who);
		if (!player->IsInvincible())
			player->TakeDamage();

		return;
	}

	if (Contains("Koopas", groups)) {
		Koopa* koopa = static_cast<Koopa*>(data.who);
		if (koopa->IsSliding()) {
			parentScene->AddEntity(new FXBoom(position));
			parentScene->AddEntity(PointUpFactory::Create(position));
			parentScene->QueueFree(this);
		}

		return;
	}
}

void PiranhaPlant::Update(float delta)
{
	Entity::Update(delta);

	if (targetPlayer == nullptr)
		targetPlayer = TryFindPlayer();

	state.Update(delta);
}

LPEntity PiranhaPlant::TryFindPlayer() {
	if (parentScene->IsEntityGroupEmpty(Group::PLAYERS))
		return nullptr;
	else {
		LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		player->GetDestroyEvent().Subscribe(this, &PiranhaPlant::OnPlayerDestroy);
		return player;
	}
}


void PiranhaPlant::PrepareCheckDistance(float delta)
{
	time += delta;

	if (time >= WAIT_DURATION) {
		time = 0;
		state.SetState(&PiranhaPlant::CheckDistance);
	}
}

void PiranhaPlant::CheckDistance(float delta)
{
	if (targetPlayer == nullptr)
		return;

	float distanceX = abs(targetPlayer->GetPosition().x - position.x);
	if (distanceX <= SAFE_DISTANCE)
		return;

	CollisionEngine::Subscribe(this, &PiranhaPlant::OnCollision, { Group::PLAYERS, "Koopas" });
	velocity.y = -MOVE_SPEED;
	state.SetState(&PiranhaPlant::MoveUp);
}

void PiranhaPlant::MoveUp(float delta)
{
	if (position.y <= stopYMoveUp) {
		position.y = stopYMoveUp;
		velocity.y = 0;
		state.SetState(&PiranhaPlant::PrepareMoveDown);
	}
}

void PiranhaPlant::PrepareMoveDown(float delta)
{
	time += delta;

	if (time >= WAIT_DURATION) {
		time = 0;
		velocity.y = MOVE_SPEED;
		state.SetState(&PiranhaPlant::MoveDown);
	}
}

void PiranhaPlant::MoveDown(float delta)
{
	if (position.y >= stopYMoveDown) {
		position.y = stopYMoveDown;
		velocity.y = 0;
		CollisionEngine::Unsubscribe(this, &PiranhaPlant::OnCollision);
		state.SetState(&PiranhaPlant::PrepareCheckDistance);
	}
}

