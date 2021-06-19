#include "BoomFX.h"
#include "Scene.h"
#include "Groups.h"
using namespace Entities;
using namespace Utils;

const float BoomFX::EXIST_DURATION = 0.2f;

BoomFX::BoomFX(const Vector2<float>& position)
	: Entity::Entity(position, "BoomFX", HitboxId::NONE, Groups::EFFECTS, GridType::NONE),
	time(0)
{
}

void BoomFX::Update(float delta)
{
	Entity::Update(delta);

	time += delta;
	if (time >= EXIST_DURATION)
		parentScene->QueueFree(this);
}
