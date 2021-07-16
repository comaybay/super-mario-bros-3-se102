#include "GoalRoulette.h"
#include "Group.h"
#include "Scene.h"
#include "Mario.h"
#include "CourseClear.h"
#include "Scene.h"
#include "CoinUp.h"
#include "FXSmoke.h"
using namespace Entities;

const float GoalRoulette::UP_SPEED = 120;
const float GoalRoulette::FLY_UP_EXIST_DURATION = 2;

GoalRoulette::GoalRoulette(const Utils::Vector2<float>& position)
	: Entity::Entity(position, "GoalRoulette", HitboxId::TILE_SIZE_HITBOX, "GoalRoulettes", GridType::MOVABLE_ENTITIES),
	time(0),
	state(EntityState<GoalRoulette>(this, &GoalRoulette::Idle))
{
}

void GoalRoulette::OnReady()
{
	Entity::OnReady();
	parentScene->UnsubscribeToOutOfWorldEvent(this);
	CollisionEngine::Subscribe(this, &GoalRoulette::OnCollision, { Group::PLAYERS });
}

void GoalRoulette::Update(float delta)
{
	Entity::Update(delta);
	state.Update(delta);
}

void GoalRoulette::Idle(float _)
{
}

void GoalRoulette::FlyUp(float delta)
{
	time += delta;
	if (time >= FLY_UP_EXIST_DURATION)
		parentScene->QueueFree(this);
}

void GoalRoulette::OnCollision(CollisionData data)
{
	CollisionEngine::Unsubscribe(this, &GoalRoulette::OnCollision);
	velocity.y = -UP_SPEED;

	switch (animation->GetCurrentFrame()) {
	case 0:
		SetAnimation("GRStar");
		parentScene->AddEntity(new CourseClear(GoalRouletteCard::STAR));
		break;
	case 1:
		SetAnimation("GRMushroom");
		parentScene->AddEntity(new CourseClear(GoalRouletteCard::MUSHROOM));
		break;
	case 2:
		SetAnimation("GRFlower");
		parentScene->AddEntity(new CourseClear(GoalRouletteCard::FLOWER));
		break;
	default:
		throw std::exception("GoalRoulette::OnCollision failed: expected index range from 0 to 2");
	}

	LPMario player = static_cast<LPMario>(data.who);
	player->StartReachedGoalRouletteAnimation();

	//turn all entities on camera to coins
	parentScene->ForEachNonWallEntityOnCamera(
		[this](LPEntity entity) -> void {
			if (Utils::Contains(Group::ENEMIES, entity->GetEntityGroups())) {
				parentScene->QueueFree(entity);
				parentScene->AddEntity(new CoinUp(entity->GetPosition(), PointType::ONE_THOUSAND_POINTS));
				parentScene->AddEntity(new FXSmoke(entity->GetPosition()));
			}
		}
	);

	state.SetState(&GoalRoulette::FlyUp);
}

