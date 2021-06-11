#include "Entities.h"
#include "Game.h"
#include "Groups.h"
using namespace Entities;

//TODO: Complete Implementation
QuestionBlock::QuestionBlock(LPEntity content, const Vector2<float>& position)
	: Entity::Entity(
		position,
		"QuestionBlockNormal",
		{ "Blocks", Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	content(content)
{
	CollisionEngine::Subscribe(this, &QuestionBlock::OnCollision, { Groups::PLAYER });
}

void QuestionBlock::OnCollision(CollisionData data)
{
	if (data.edge.y == -1.0f) {
		LPEntityManager entityManager = Game::GetSceneEntityManager();
		entityManager->QueueFree(this);
		entityManager->Add(content);
		content = nullptr;
	}
}


void QuestionBlock::Update(float delta)
{
	Entity::Update(delta);
}

