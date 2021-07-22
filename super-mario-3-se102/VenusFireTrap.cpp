#include "VenusFireTrap.h"
#include "Color.h"
#include "Group.h"
#include "Constants.h"
#include "EntityUtils.h"
#include "Scene.h"
#include "Mario.h"
#include "FXBoom.h"
#include "PointUpFactory.h"
#include "VenusFireBall.h"
using namespace Entities;
using namespace Utils;

const float VenusFireTrap::OFFSET_ANGLE_LOW = 10;
const float VenusFireTrap::OFFSET_ANGLE_HIGH = 20;
const float VenusFireTrap::MAX_LOW_ANGLE_RIGHT = -45;
const float VenusFireTrap::MAX_LOW_ANGLE_LEFT = -135;
const float VenusFireTrap::WAIT_DURATION = 1;
const float VenusFireTrap::MOVE_SPEED = 60;
const float VenusFireTrap::SAFE_DISTANCE = Constants::TILE_SIZE * 2.0f;

VenusFireTrap::VenusFireTrap(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(
		{ position.x, position.y + Constants::TILE_SIZE * 2 },
		AnimationId::NONE, "HitboxVenusFireTrap", { "VenusFireTraps" , Group::ENEMIES }, GridType::MOVABLE_ENTITIES, true),
	colorCode(Color::ToColorCode(colorType)),
	stopYMoveUp(position.y),
	stopYMoveDown(position.y + Constants::TILE_SIZE * 2),
	targetPlayer(nullptr),
	time(0),
	state(EntityState<VenusFireTrap>(this, &VenusFireTrap::CheckDistance))
{
}

void VenusFireTrap::OnReady()
{
	Entity::OnReady();
	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		targetPlayer = parentScene->GetEntityOfGroup(Group::PLAYERS);
		targetPlayer->GetDestroyEvent().Subscribe(this, &VenusFireTrap::OnPlayerDestroy);
	}
}

void VenusFireTrap::OnPlayerDestroy(LPEntity entity) {
	targetPlayer = nullptr;
}

void VenusFireTrap::OnCollision(CollisionData data)
{
	LPMario player = static_cast<LPMario>(data.who);
	if (!player->IsInvincible())
		player->TakeDamage();
}

void VenusFireTrap::Update(float delta)
{
	Entity::Update(delta);

	if (targetPlayer == nullptr)
		targetPlayer = TryFindPlayer();

	state.Update(delta);
}

void VenusFireTrap::GetKnockedOver(HDirection direction)
{
	parentScene->AddEntity(new FXBoom(position));
	parentScene->AddEntity(PointUpFactory::Create(position));
	parentScene->QueueFree(this);
}

LPEntity VenusFireTrap::TryFindPlayer() {
	if (parentScene->IsEntityGroupEmpty(Group::PLAYERS))
		return nullptr;

	LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
	player->GetDestroyEvent().Subscribe(this, &VenusFireTrap::OnPlayerDestroy);
	return player;
}

void VenusFireTrap::PrepareCheckDistance(float delta)
{
	time += delta;

	if (time >= WAIT_DURATION) {
		time = 0;
		state.SetState(&VenusFireTrap::CheckDistance);
	}
}

void VenusFireTrap::CheckDistance(float delta)
{
	if (targetPlayer == nullptr)
		return;

	float distanceX = abs(targetPlayer->GetPosition().x - position.x);
	if (distanceX <= SAFE_DISTANCE)
		return;

	SetDetectable(true);
	CollisionEngine::Subscribe(this, &VenusFireTrap::OnCollision, { Group::PLAYERS });
	velocity.y = -MOVE_SPEED;
	state.SetState(&VenusFireTrap::MoveUp);
}

void VenusFireTrap::MoveUp(float delta)
{
	UpdateHeadAnimation();

	if (position.y <= stopYMoveUp) {
		position.y = stopYMoveUp;
		velocity.y = 0;
		state.SetState(&VenusFireTrap::WaitAndShoot);
	}
}

void VenusFireTrap::WaitAndShoot(float delta)
{
	UpdateHeadAnimation();

	time += delta;

	if (time >= WAIT_DURATION) {
		time = 0;

		if (targetPlayer != nullptr) {
			const Vector2<float>& playerPos = targetPlayer->GetPosition();
			float angle = AngleToXAxis(playerPos, position);

			if (EntityUtils::IsAboveOf(this, targetPlayer)) {
				if (EntityUtils::IsOnLeftSideOf(this, targetPlayer))
					angle -= OFFSET_ANGLE_HIGH;
				else
					angle += OFFSET_ANGLE_HIGH;
			}
			else {
				if (EntityUtils::IsOnLeftSideOf(this, targetPlayer))
					angle += OFFSET_ANGLE_LOW;
				else
					angle -= OFFSET_ANGLE_LOW;
			}

			if (angle < 0 && angle < MAX_LOW_ANGLE_RIGHT && angle > MAX_LOW_ANGLE_LEFT)
				angle = angle > -90 ? MAX_LOW_ANGLE_RIGHT : MAX_LOW_ANGLE_LEFT;


			parentScene->AddEntity(new VenusFireBall(position, angle));
		}

		state.SetState(&VenusFireTrap::PrepareMoveDown);
	}
}

void VenusFireTrap::PrepareMoveDown(float delta)
{
	UpdateHeadAnimation();

	time += delta;

	if (time >= WAIT_DURATION) {
		time = 0;
		velocity.y = MOVE_SPEED;
		state.SetState(&VenusFireTrap::MoveDown);
	}
}

void VenusFireTrap::MoveDown(float delta)
{
	UpdateHeadAnimation();

	if (position.y >= stopYMoveDown) {
		position.y = stopYMoveDown;
		velocity.y = 0;
		SetDetectable(false);
		CollisionEngine::Unsubscribe(this, &VenusFireTrap::OnCollision);
		state.SetState(&VenusFireTrap::PrepareCheckDistance);
	}
}

void VenusFireTrap::UpdateHeadAnimation() {
	if (targetPlayer != nullptr) {
		bool left = EntityUtils::IsOnLeftSideOf(this, targetPlayer);
		bool above = EntityUtils::IsAboveOf(this, targetPlayer);

		if (left && above)
			SetAnimation(colorCode + "VenusUL");

		else if (!left && above)
			SetAnimation(colorCode + "VenusUR");

		else if (left && !above)
			SetAnimation(colorCode + "VenusDL");

		else if (!left && !above)
			SetAnimation(colorCode + "VenusDR");
	};

	bool waiting = state.GetState() == &VenusFireTrap::WaitAndShoot || state.GetState() == &VenusFireTrap::PrepareMoveDown;
	if (waiting == true)
		animation->SetFrame(0);
}
