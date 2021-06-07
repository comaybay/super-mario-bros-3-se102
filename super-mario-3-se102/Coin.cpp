#include "Entities.h"
#include "Groups.h"
#include "EntityManager.h"
#include "PlayerVariables.h"
using namespace Entities;

Coin::Coin(const Vector2& position)
	: Entity::Entity(position, "Coin", "Coins")
{
	CollisionDetection::Subscribe(this, &Coin::OnCollision, { Groups::PLAYER });
}

void Entities::Coin::OnCollision(CollisionData data)
{
	PlayerVariables::AddToNumberOfCoins(1);
	EntityManager::QueueFree(this);
}

