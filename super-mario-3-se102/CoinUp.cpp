#include "CoinUp.h"
#include "Group.h"
#include "EntityConstants.h"
#include "Scene.h"
using namespace Entities;

const float CoinUp::UP_SPEED = 300;
const float CoinUp::FALL_SPEED = EntityConstants::GRAVITY * 1.25f;
const float CoinUp::END_TIME = 0.7;

CoinUp::CoinUp(const Utils::Vector2<float>& position)
	: Entity::Entity(position, "CoinUp", HitboxId::NONE, Group::EFFECTS, GridType::NONE),
	pointUpType(PointUp::Type::ONE_HUNDRED_POINTS),
	time(0)
{
	velocity.y = -UP_SPEED;
}

CoinUp::CoinUp(const Utils::Vector2<float>& position, PointUp::Type pointUpType)
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
	if (time < END_TIME)
		return;

	parentScene->QueueFree(this);
	parentScene->AddEntity(new PointUp(position, pointUpType));
}

