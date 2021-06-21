#include "QuestionBlock.h"
#include "Group.h"
#include "EntityManager.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

//TODO: Complete Implementation
QuestionBlock::QuestionBlock(LPEntity content, const Vector2<float>& position)
	: Entity::Entity(
		position,
		"QuestionBlockNormal",
		HitboxId::TILE_SIZE_HITBOX,
		{ "Blocks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	content(content)
{
	CollisionEngine::Subscribe(this, &QuestionBlock::OnCollision, { Group::PLAYER });
}

void QuestionBlock::OnCollision(CollisionData data)
{
	if (data.edge.y == -1.0f) {
		parentScene->QueueFree(this);
		parentScene->AddEntity(content);
		content = nullptr;
	}
}

void QuestionBlock::Update(float delta)
{
	Entity::Update(delta);
}

