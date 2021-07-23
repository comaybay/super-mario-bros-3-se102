#include "CoinUp.h"
#include "Group.h"
#include "EntityConstants.h"
#include "Scene.h"
using namespace Entities;

const float CoinUp::UP_SPEED = 300;
const float CoinUp::FALL_SPEED = EntityConstants::GRAVITY * 1.1f;
const float CoinUp::EXIST_DURATION = 0.7f;

CoinUp::CoinUp(const Utils::Vector2<float>& position)
	: Entity::Entity(position, "CoinUp", HitboxId::NONE, Group::EFFECTS, GridType::NONE),
	pointUpType(PointType::ONE_HUNDRED_POINTS),
	time(0)
{
	PlayerVariables::AddToNumberOfCoins(1);
	velocity.y = -UP_SPEED;
}

CoinUp::CoinUp(const Utils::Vector2<float>& position, PointType pointUpType)
	: Entity::Entity(position, "CoinUp", HitboxId::NONE, Group::EFFECTS, GridType::NONE),
	pointUpType(pointUpType),
	time(0)
{
	velocity.y = -UP_SPEED;
}

void CoinUp::Update(float delta) {
	Entity::Update(delta);

	velocity.y += FALL_SPEED * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	time += delta;
	if (time < EXIST_DURATION)
		return;

	parentScene->QueueFree(this);
	parentScene->AddEntity(new PointUp(position, pointUpType));
}

