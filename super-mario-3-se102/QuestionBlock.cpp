#include "Entities.h"
#include "EntityManager.h"
#include "Groups.h"
using namespace Entities;

//TODO: Complete Implementation
QuestionBlock::QuestionBlock(LPEntity content, const Vector2& position)
	: Entity::Entity(position, "QuestionBlockNormal", { "Blocks", Groups::COLLISION_WALLS, Groups::COLLISION_WALLS_TYPE_1 }),
	content(content)
{
	CollisionDetection::Subscribe(this, &QuestionBlock::OnCollision, { Groups::PLAYER });
}

void QuestionBlock::OnCollision(CollisionData data)
{
	if (data.edge.y == -1.0f) {
		EntityManager::QueueFree(this);
		EntityManager::AddToGroups(content->GetEntityGroups(), content);
		content = nullptr;
	}
}


void QuestionBlock::Update(float delta)
{
	Entity::Update(delta);
}

