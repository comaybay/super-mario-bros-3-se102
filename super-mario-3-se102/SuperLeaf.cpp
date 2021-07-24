#include "SuperLeaf.h"
#include "EntityUtils.h"
#include "Group.h"
#include "PointUpFactory.h"
#include "Scene.h"
#include "MarioTransition.h"

using namespace Entities;
using namespace Utils;

float SuperLeaf::MOVE_SPEED = 90;
float SuperLeaf::UP_SPEED = 80;
float SuperLeaf::ACCEL = 300;
float SuperLeaf::FALL_SPEED = 90;
float SuperLeaf::MAX_FALL_SPEED = 30;

SuperLeaf::SuperLeaf(const Utils::Vector2<float>& position)
	:Entity(position, "SuperLeafL", HitboxId::TILE_SIZE_HITBOX, "SuperLeafs", GridType::MOVABLE_ENTITIES),
	state(this, &SuperLeaf::FlyUp),
	time(0),
	midX(position.x + Constants::TILE_SIZE),
	dir(1)
{
	velocity.y = -UP_SPEED;
}

void SuperLeaf::Update(float delta) {
	Entity::Update(delta);
	state.Update(delta);
}

void SuperLeaf::OnReady()
{
	CollisionEngine::Subscribe(this, &SuperLeaf::OnCollision, { Group::PLAYERS });
}

void SuperLeaf::OnCollision(CollisionData data)
{
	Vector2<float> pointPos = { position.x, position.y - Constants::TILE_SIZE };
	parentScene->AddEntity(new PointUp(pointPos, PointType::ONE_THOUSAND_POINTS));

	parentScene->AddEntity(new MarioTransition(static_cast<LPMario>(data.who), PlayerPowerLevel::RACCOON));

	parentScene->QueueFree(this);
}

void SuperLeaf::FlyUp(float delta)
{
	velocity.y = velocity.y + FALL_SPEED * delta;

	if (velocity.y > 0) {
		velocity = { MOVE_SPEED , 0 };
		state.SetState(&SuperLeaf::Fall);
	}
}

void SuperLeaf::Fall(float delta)
{
	time += delta;
	velocity.y += FALL_SPEED * delta;
	velocity.x = Clip(velocity.x + ACCEL * delta * dir, -MOVE_SPEED, MOVE_SPEED);

	if (dir == 1 && position.x > midX) {
		velocity.y = 0;
		dir = -1;
	}

	else if (dir == -1 && position.x < midX) {
		velocity.y = 0;
		dir = 1;
	}

	SetAnimation(velocity.x < 0 ? "SuperLeafL" : "SuperLeafR");

}

