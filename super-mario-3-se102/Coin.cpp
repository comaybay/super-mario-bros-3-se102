#include "Entities.h"
#include "EntityManager.h"
#include "Group.h"
#include "PlayerVariables.h"
#include "Game.h"
using namespace Entities;
using namespace Utils;

Coin::Coin(const Vector2<float>& position)
	: Entity::Entity(position, "Coin", HitboxId::TILE_SIZE_HITBOX, "Coins", GridType::STATIC_ENTITIES)
{
}

void Coin::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Coin::OnCollision, { Group::PLAYERS });
}


void Coin::OnCollision(CollisionData data)
{
	PlayerVariables::AddToNumberOfCoins(1);
	parentScene->QueueFree(this);
}

