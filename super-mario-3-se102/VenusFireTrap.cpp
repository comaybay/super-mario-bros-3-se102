#include "VenusFireTrap.h"
#include "Color.h"
#include "Group.h"
#include "Constants.h"
#include "EntityUtils.h"
#include "Scene.h"
using namespace Entities;

const float VenusFireTrap::WAIT_DURATION = 1;
const float VenusFireTrap::MOVE_SPEED = 60;
const float VenusFireTrap::SAFE_DISTANCE = Constants::TILE_SIZE * 2;

VenusFireTrap::VenusFireTrap(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(
		{ position.x, position.y + Constants::TILE_SIZE * 2 },
		AnimationId::NONE, "HitboxVenusFireTrap", { "VenusFireTraps" , Group::ENEMIES }, GridType::MOVABLE_ENTITIES, true),
	colorCode(Color::ToColorCode(colorType)),
	stopYMoveUp(position.y),
	stopYMoveDown(position.y + Constants::TILE_SIZE * 2),
	player(nullptr),
	state(EntityState<VenusFireTrap>(this, &VenusFireTrap::PrepareCheckDistance))
{
}

void VenusFireTrap::OnReady()
{
	CollisionEngine::Subscribe(this, &VenusFireTrap::OnCollision, { Group::PLAYERS, "Koopas" });

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		player->GetDestroyEvent().Subscribe(this, &VenusFireTrap::OnPlayerDestroy);
	}
}

void VenusFireTrap::OnPlayerDestroy(LPEntity entity) {
	player = nullptr;
}

void VenusFireTrap::OnCollision(CollisionData data)
{
}

void VenusFireTrap::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
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
	if (player != nullptr) {
		float distanceX = abs(player->GetPosition().x - position.x);
		if (distanceX <= SAFE_DISTANCE)
			return;

		velocity.y = -MOVE_SPEED;
		state.SetState(&VenusFireTrap::MoveUp);
	}

	if (player == nullptr && !parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		player->GetDestroyEvent().Subscribe(this, &VenusFireTrap::OnPlayerDestroy);
	}
}

void VenusFireTrap::MoveUp(float delta)
{
	if (player != nullptr)
		UpdateHeadAnimation();

	if (position.y <= stopYMoveUp) {
		position.y = stopYMoveUp;
		velocity.y = 0;
		state.SetState(&VenusFireTrap::WaitAndShoot);
	}
}

void VenusFireTrap::WaitAndShoot(float delta)
{
	if (player != nullptr)
		UpdateHeadAnimation();

	time += delta;

	if (time >= WAIT_DURATION) {
		time = 0;
		state.SetState(&VenusFireTrap::PrepareMoveDown);
	}
}

void VenusFireTrap::PrepareMoveDown(float delta)
{
	if (player != nullptr)
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
	if (player != nullptr)
		UpdateHeadAnimation();

	if (position.y >= stopYMoveDown) {
		position.y = stopYMoveDown;
		velocity.y = 0;
		state.SetState(&VenusFireTrap::PrepareCheckDistance);
	}
}

void VenusFireTrap::UpdateHeadAnimation() {
	bool left = EntityUtils::IsOnLeftSideOf(this, player);
	bool above = EntityUtils::IsOnAboveOf(this, player);
	if (left && above)
		SetAnimation("RVenusUL");

	else if (!left && above)
		SetAnimation("RVenusUR");

	else if (left && !above)
		SetAnimation("RVenusDL");

	else if (!left && !above)
		SetAnimation("RVenusDR");

	bool waiting = state.GetState() == &VenusFireTrap::WaitAndShoot || state.GetState() == &VenusFireTrap::PrepareMoveDown;
	if (waiting == true)
		animation->SetFrame(0);
}
