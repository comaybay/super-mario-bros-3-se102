#include "FXBoom.h"
#include "Scene.h"
#include "Group.h"
using namespace Entities;
using namespace Utils;

const float FXBoom::EXIST_DURATION = 0.2f;

FXBoom::FXBoom(const Vector2<float>& position)
	: Entity::Entity(position, "FXBoom", HitboxId::NONE, Group::EFFECTS, GridType::NONE),
	time(0)
{
}

void FXBoom::Update(float delta)
{
	Entity::Update(delta);

	time += delta;
	if (time >= EXIST_DURATION)
		parentScene->QueueFree(this);
}
