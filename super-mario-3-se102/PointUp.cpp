#include "PointUp.h"
#include "Group.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

const float PointUp::EXIST_DURATION = 0.8f;
const float PointUp::MOVE_UP_SPEED = 110.0f;
const float PointUp::FRICTION = 5.0f;

const std::unordered_map<PointType, std::string> PointUp::animationIdByType{
	{PointType::ONE_HUNDRED_POINTS, "100P"},
	{PointType::TWO_HUNDRED_POINTS, "200P"},
	{PointType::FOUR_HUNDRED_POINTS, "400P"},
	{PointType::EIGHT_HUNDRED_POINTS, "800P"},
	{PointType::ONE_THOUSAND_POINTS, "1000P"},
	{PointType::TWO_THOUSAND_POINTS, "2000P"},
	{PointType::FOUR_THOUSAND_POINTS, "4000P"},
	{PointType::EIGHT_THOUSAND_POINTS, "8000P"},
	{PointType::ONE_UP, "1UP"}
};

const std::unordered_map<PointType, int> PointUp::scoreByType{
	{PointType::ONE_HUNDRED_POINTS, 100},
	{PointType::TWO_HUNDRED_POINTS, 200},
	{PointType::FOUR_HUNDRED_POINTS, 400},
	{PointType::EIGHT_HUNDRED_POINTS, 800},
	{PointType::ONE_THOUSAND_POINTS, 1000},
	{PointType::TWO_THOUSAND_POINTS, 2000},
	{PointType::FOUR_THOUSAND_POINTS, 4000},
	{PointType::EIGHT_THOUSAND_POINTS, 8000},
};

PointUp::PointUp(const Vector2<float>& position, PointType pointType)
	: Entity::Entity(position, Group::EFFECTS, GridType::NONE),
	time(0)
{
	if (pointType != PointType::ONE_UP)
		PlayerVariables::AddToScore(scoreByType.at(pointType));
	else
		PlayerVariables::AddToNumberOfLives(1);

	velocity.y = -MOVE_UP_SPEED;
	SetAnimation(animationIdByType.at(pointType));
}

void PointUp::Update(float delta) {
	Entity::Update(delta);
	velocity.y += FRICTION;
	velocity.y = min(velocity.y, 0);

	time += delta;
	if (time >= EXIST_DURATION)
		parentScene->QueueFree(this);
}

