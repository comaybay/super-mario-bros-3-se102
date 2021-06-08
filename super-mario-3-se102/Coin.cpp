#include "Entities.h"
#include "Groups.h"
#include "PlayerVariables.h"
#include "Game.h"
using namespace Entities;

Coin::Coin(const Vector2& position)
	: Entity::Entity(position, "Coin", "Coins")
{
	CollisionEngine::Subscribe(this, &Coin::OnCollision, { Groups::PLAYER });
}

void Entities::Coin::OnCollision(CollisionData data)
{
	PlayerVariables::AddToNumberOfCoins(1);
	Game::GetSceneEntityManager()->QueueFree(this);
}

