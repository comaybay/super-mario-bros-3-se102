#include "FXSmoke.h"
#include "Scene.h"
#include "Group.h"
using namespace Entities;
using namespace Utils;

const float FXSmoke::EXIST_DURATION = 0.48f;

FXSmoke::FXSmoke(const Vector2<float>& position)
	: Entity::Entity(position, "FXSmoke", HitboxId::NONE, Group::EFFECTS, GridType::NONE),
	time(0)
{
}

void FXSmoke::Update(float delta)
{
	Entity::Update(delta);

	time += delta;
	if (time >= EXIST_DURATION)
		parentScene->QueueFree(this);
}
