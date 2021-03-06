#include "QuestionBlock.h"
#include "EmptyBlock.h"
#include "Group.h"
#include "EntityManager.h"
#include "ContentFactory.h"
#include "Contains.h"
#include "Constants.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

QuestionBlock::QuestionBlock(const Vector2<float>& position)
	: Entity::Entity(
		position,
		"QuestionBlockNormal",
		HitboxId::TILE_SIZE_HITBOX,
		{ "Blocks",  Group::BLOCKS, Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	contentId(ContentId::NONE)
{
}

QuestionBlock::QuestionBlock(const std::string& contentId, const Vector2<float>& position)
	: Entity::Entity(
		position,
		"QuestionBlockNormal",
		HitboxId::TILE_SIZE_HITBOX,
		{ "Blocks",  Group::BLOCKS, Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	contentId(contentId)
{
}

void QuestionBlock::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &QuestionBlock::OnCollision, { Group::PLAYERS });
}

void QuestionBlock::GetKnockedOver(HDirection direction)
{
	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS))
		ExposeContent(static_cast<LPMario>(parentScene->GetEntityOfGroup(Group::PLAYERS)));
}

void QuestionBlock::OnCollision(CollisionData data)
{
	if (data.edge.y == -1)
		ExposeContent(static_cast<LPMario>(data.who));
}

void QuestionBlock::ExposeContent(LPMario player)
{
	if (contentId != ContentId::NONE) {
		Vector2<float> contentPos = { position.x, position.y - Constants::TILE_SIZE };
		LPEntity content = ContentFactory(player).Create(contentId, contentPos);
		parentScene->AddEntity(content);
	}

	parentScene->QueueFree(this);
	parentScene->AddEntity(new EmptyBlock(position));
}


