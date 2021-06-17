#include "Entities.h"
#include "EntityManager.h"
#include "Groups.h"
#include "PlayerVariables.h"
#include "Game.h"
using namespace Entities;
using namespace Utils;

Coin::Coin(const Vector2<float>& position)
	: Entity::Entity(position, "Coin", "Coins", GridType::STATIC_ENTITIES)
{
	CollisionEngine::Subscribe(this, &Coin::OnCollision, { Groups::PLAYER });
}

void Coin::OnCollision(CollisionData data)
{
	PlayerVariables::AddToNumberOfCoins(1);
	parentScene->QueueFree(this);
}

