#include "PointUp.h"
#include "Group.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

const float PointUp::EXIST_DURATION = 0.8f;
const float PointUp::MOVE_UP_SPEED = 110.0f;
const float PointUp::FRICTION = 5.0f;

//TODO: Implement all point up animations for this
const std::unordered_map<PointUp::Type, std::string> PointUp::animationIdByType{
	{Type::ONE_HUNDRED_POINTS, "100P"},
	{Type::TWO_HUNDRED_POINTS, "200P"},
	{Type::FOUR_HUNDRED_POINTS, "400P"},
	{Type::EIGHT_HUNDRED_POINTS, "800P"},
	{Type::ONE_THOUSAND_POINTS, "1000P"},
	{Type::TWO_THOUSAND_POINTS, "2000P"},
	{Type::FOUR_THOUSAND_POINTS, "4000P"},
	{Type::EIGHT_THOUSAND_POINTS, "8000P"},
	{Type::ONE_UP, "1UP"}
};


PointUp::PointUp(const Vector2<float>& position, Type pointType)
	: Entity::Entity(position, Group::EFFECTS, GridType::NONE),
	time(0)
{
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

