#include "MarioSpinAttack.h"
#include "Group.h"
#include "IKnockedOverable.h"
#include "Scene.h"

using namespace Entities;

const float MarioSpinAttack::SPEED = 80;
const float MarioSpinAttack::MAX_DISTANCE = 14;

MarioSpinAttack::MarioSpinAttack(LPEntity player, HDirection dir)
	: Entity(player->GetPosition(), AnimationId::NONE, "HitboxSpinAttack", "MarioSpinAttacks", GridType::NONE),
	offsetPosX(0),
	player(player)
{
	const Hitbox& hbox = player->GetHitbox();
	hitboxOffsetX = hbox.GetRelativePosition().x + 1;
	if (dir == HDirection::RIGHT)
		hitboxOffsetX += hbox.GetDimension().width - 2;

	position.x += hitboxOffsetX;

	velocity.x = dir == HDirection::LEFT ? -SPEED : SPEED;
}

void MarioSpinAttack::OnReady() {
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &MarioSpinAttack::OnCollision, { Group::ENEMIES, Group::BLOCKS });
	player->GetDestroyEvent().Subscribe(this, &MarioSpinAttack::OnPlayerDestroy);
}

void MarioSpinAttack::OnCollision(CollisionData data) {
	if (IKnockedOverable* enemy = dynamic_cast<IKnockedOverable*>(data.who))
		enemy->GetKnockedOver(data.edge.x == 1.0f ? HDirection::LEFT : HDirection::RIGHT);
}

void MarioSpinAttack::OnPlayerDestroy(LPEntity _)
{
	parentScene->QueueFree(this);
}

void MarioSpinAttack::Update(float delta)
{
	Entity::Update(delta);

	if (abs(offsetPosX) + abs(velocity.x * delta) > MAX_DISTANCE)
		parentScene->QueueFree(this);

	velocity.y = min(player->GetPosition().y - position.y, 0);

	position = player->GetPosition();
	position.x += hitboxOffsetX + offsetPosX;
	offsetPosX += velocity.x * delta;
}

