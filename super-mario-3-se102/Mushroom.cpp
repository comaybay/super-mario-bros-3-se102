#include "Mushroom.h"
#include "Scene.h"
#include "Group.h"
#include "Mario.h"
#include "Constants.h"
using namespace Entities;
using namespace Utils;

const float Mushroom::RISE_SPEED = 20;
const float Mushroom::MOVE_SPEED = 60;

Mushroom::Mushroom(const Vector2<float>& position)
	: Entity(position, "Mushroom", HitboxId::TILE_SIZE_HITBOX, { "Mushrooms", Group::ITEMS }, GridType::NONE),
	state(EntityState<Mushroom>(this, &Mushroom::RiseUp)),
	riseDestY(position.y)
{
	this->position.y += Constants::TILE_SIZE;
	velocity.y = -RISE_SPEED;
}

void Mushroom::Update(float delta) {
	Entity::Update(delta);
	state.Handle(delta);
}

void Mushroom::RiseUp(float delta)
{
	if (position.y > riseDestY)
		return;

	position.y = riseDestY;
	state.SetHandler(&Mushroom::Move);

	const std::list<LPEntity>& playerGroup = parentScene->GetEntitiesByGroup(Group::PLAYER);
	if (!playerGroup.empty()) {
		Mario* player = static_cast<Mario*>(playerGroup.front());
		velocity.x = (player->GetPosition().x < position.x) ? -MOVE_SPEED : MOVE_SPEED;
	}
	else
		velocity.x = -MOVE_SPEED;
}

void Mushroom::Move(float delta)
{
}
