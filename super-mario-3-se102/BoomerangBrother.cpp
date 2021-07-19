#include "BoomerangBrother.h"
#include "Constants.h"
#include "EntityConstants.h"
#include "Group.h"
#include "Scene.h"
#include "EntityUtils.h"
#include "Mario.h"
#include "FXBoom.h"
#include "PointUpFactory.h"
#include "CollisionHandling.h"
#include "PointUp.h"
using namespace Entities;
using namespace Utils;

const int BoomerangBrother::NUMBER_OF_NORMAL_TURNS = 2;
const int BoomerangBrother::NUMBER_OF_THROW_TURNS = 2;
const float BoomerangBrother::WAIT_DURATION = 0.5f;
const float BoomerangBrother::DURATION_TILL_THROW = 0.3f;
const float BoomerangBrother::MOVE_SPEED = 30;
const float BoomerangBrother::JUMP_SPEED = 200;
const int BoomerangBrother::JUMP_CHANCE = 33;
const Utils::Vector2<float> BoomerangBrother::BOOMERANG_OFFSET_LEFT = { 10, -4 };
const Utils::Vector2<float> BoomerangBrother::BOOMERANG_OFFSET_RIGHT = { -10, -4 };

BoomerangBrother::BoomerangBrother(const Vector2<float>& position)
	: Entity::Entity(position, "BoomerangBroL", "HitboxBoomerangBrother", { "BoomerangBrothers", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	stopXMoveLeft(position.x - Constants::TILE_SIZE * 2),
	stopXMoveRight(position.x),
	prepareThrowX(position.x - Constants::TILE_SIZE),
	boomerang(nullptr),
	targetPlayer(nullptr),
	time(0),
	turnCount(0),
	hasThrownThisTurn(false),
	moveDir(HDirection::LEFT),
	state(this, &BoomerangBrother::MoveLeft),
	knockedOverMovement(nullptr)
{
	velocity.x = -MOVE_SPEED;
}

void BoomerangBrother::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &BoomerangBrother::OnCollision, { Group::PLAYERS, Group::COLLISION_WALLS });
}

void BoomerangBrother::Update(float delta) {
	Entity::Update(delta);

	if (targetPlayer == nullptr)
		targetPlayer = TryFindPlayer();

	state.Update(delta);
}

LPEntity BoomerangBrother::TryFindPlayer() {
	if (parentScene->IsEntityGroupEmpty(Group::PLAYERS))
		return nullptr;

	LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
	player->GetDestroyEvent().Subscribe(this, &BoomerangBrother::OnPlayerDestroy);
	return player;
}

void BoomerangBrother::OnPlayerDestroy(LPEntity _)
{
	targetPlayer = nullptr;
}

void BoomerangBrother::MoveLeft(float delta)
{
	UpdateAnimation();

	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);

	if (!hasTryJumpThisTurn && position.x <= prepareThrowX) {
		hasTryJumpThisTurn = true;
		if (Chance(JUMP_CHANCE))
			velocity.y = -JUMP_SPEED;
	}

	if (position.x <= stopXMoveLeft) {
		position.x = stopXMoveLeft;
		velocity.x = 0;
		animation->SetAnimationSpeed(0);
		state.SetState(&BoomerangBrother::Wait);
	}

}

void BoomerangBrother::MoveRight(float delta)
{
	UpdateAnimation();

	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);

	if (!hasTryJumpThisTurn && position.x >= prepareThrowX) {
		hasTryJumpThisTurn = true;
		if (Chance(JUMP_CHANCE))
			velocity.y = -JUMP_SPEED;
	}

	if (position.x >= stopXMoveRight) {
		position.x = stopXMoveRight;
		velocity.x = 0;
		animation->SetAnimationSpeed(0);
		state.SetState(&BoomerangBrother::Wait);
	}

}

void BoomerangBrother::MoveAndThrowLeft(float delta)
{
	MoveLeft(delta);

	if (hasThrownThisTurn || position.x > prepareThrowX)
		return;

	if (boomerang == nullptr) {
		boomerang = new Boomerang(this, BOOMERANG_OFFSET_LEFT, HDirection::LEFT);
		parentScene->AddEntity(boomerang);

	}

	bool playerOnLeftSide = EntityUtils::IsOnLeftSideOf(this, targetPlayer);
	boomerang->SetDirection(playerOnLeftSide ? HDirection::LEFT : HDirection::RIGHT);
	boomerang->SetOffset(playerOnLeftSide ? BOOMERANG_OFFSET_LEFT : BOOMERANG_OFFSET_RIGHT);

	UpdateAnimationPrepareThrow();
	time += delta;
	if (time >= DURATION_TILL_THROW) {
		boomerang->StartMoving();
		boomerang = nullptr;
		time = 0;
		hasThrownThisTurn = true;
	}
}

void BoomerangBrother::MoveAndThrowRight(float delta)
{
	MoveRight(delta);

	if (hasThrownThisTurn || position.x < prepareThrowX)
		return;

	if (boomerang == nullptr) {
		boomerang = new Boomerang(this, BOOMERANG_OFFSET_RIGHT, HDirection::RIGHT);
		parentScene->AddEntity(boomerang);
	}

	bool playerOnLeftSide = EntityUtils::IsOnLeftSideOf(this, targetPlayer);
	boomerang->SetDirection(playerOnLeftSide ? HDirection::LEFT : HDirection::RIGHT);
	boomerang->SetOffset(playerOnLeftSide ? BOOMERANG_OFFSET_LEFT : BOOMERANG_OFFSET_RIGHT);

	UpdateAnimationPrepareThrow();
	time += delta;
	if (time >= DURATION_TILL_THROW) {
		boomerang->StartMoving();
		boomerang = nullptr;
		time = 0;
		hasThrownThisTurn = true;
	}
}

void BoomerangBrother::Wait(float delta)
{
	UpdateAnimation();

	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);

	time += delta;
	if (time >= WAIT_DURATION) {
		time = 0;
		turnCount = (turnCount + 1) % (NUMBER_OF_NORMAL_TURNS + NUMBER_OF_THROW_TURNS);
		hasThrownThisTurn = false;
		hasTryJumpThisTurn = false;

		moveDir = moveDir == HDirection::LEFT ? HDirection::RIGHT : HDirection::LEFT;
		velocity.x = moveDir == HDirection::LEFT ? -MOVE_SPEED : MOVE_SPEED;

		if (turnCount > NUMBER_OF_NORMAL_TURNS)
			state.SetState(moveDir == HDirection::LEFT ? &BoomerangBrother::MoveLeft : &BoomerangBrother::MoveRight);
		else
			state.SetState(moveDir == HDirection::LEFT ? &BoomerangBrother::MoveAndThrowLeft : &BoomerangBrother::MoveAndThrowRight);
	}
}


void BoomerangBrother::KnockedOver(float delta)
{
	knockedOverMovement->Update(delta);

	if (knockedOverMovement->Finished())
		parentScene->QueueFree(this);
}

void BoomerangBrother::StompedOn(float delta)
{
	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);
}

void BoomerangBrother::UpdateAnimation()
{
	if (targetPlayer == nullptr)
		return;

	if (EntityUtils::IsOnLeftSideOf(this, targetPlayer))
		SetAnimation("BoomerangBroL");
	else
		SetAnimation("BoomerangBroR");

	if (state.GetState() == &BoomerangBrother::Wait)
		animation->SetAnimationSpeed(0);
}

void BoomerangBrother::UpdateAnimationPrepareThrow() {
	if (hasThrownThisTurn || targetPlayer == nullptr)
		return;

	bool playerOnLeftSide = EntityUtils::IsOnLeftSideOf(this, targetPlayer);

	if (playerOnLeftSide)
		SetAnimation("BoomerangBroHoldL");
	else
		SetAnimation("BoomerangBroHoldR");

}


void BoomerangBrother::OnCollision(CollisionData data) {
	const EntityGroups& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups)) {
		LPMario player = static_cast<LPMario>(data.who);

		if (data.edge.x != 0 && !player->IsInvincible()) {
			player->TakeDamage();
			return;
		}

		if (data.edge.y == 1) {
			SetDetectable(false);
			CollisionEngine::Unsubscribe(this, &BoomerangBrother::OnCollision);

			if (boomerang != nullptr)
				parentScene->QueueFree(boomerang);

			player->Bounce();

			SetAnimation(EntityUtils::IsOnLeftSideOf(this, player) ? "BoomerangBroKOL" : "BoomerangBroKOR");
			state.SetState(&BoomerangBrother::StompedOn);
			parentScene->AddEntity(new PointUp(position, PointType::ONE_THOUSAND_POINTS));
			return;
		}

		return;
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y != 0) {
			velocity.y = 0;
			return;
		}

		if (data.edge.x != 0) {
			velocity.x = MOVE_SPEED * data.edge.x;
			return;
		}

		return;
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1) {
		CollisionHandling::Slide(this, data);
		return;
	}
}

void BoomerangBrother::GetKnockedOver(HDirection direction)
{
	SetDetectable(false);
	CollisionEngine::Unsubscribe(this, &BoomerangBrother::OnCollision);

	if (targetPlayer != nullptr)
		SetAnimation(EntityUtils::IsOnLeftSideOf(this, targetPlayer) ? "BoomerangBroKOL" : "BoomerangBroKOR");

	if (boomerang != nullptr)
		parentScene->QueueFree(boomerang);

	knockedOverMovement = new MovementKnockedOver(this, direction);
	state.SetState(&BoomerangBrother::KnockedOver);

	parentScene->AddEntity(new FXBoom(position));
	parentScene->AddEntity(PointUpFactory::Create(position));
}