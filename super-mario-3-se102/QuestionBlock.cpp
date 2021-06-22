#include "QuestionBlock.h"
#include "Koopa.h"
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
	CollisionEngine::Subscribe(this, &QuestionBlock::OnCollision, { Group::PLAYER, "Koopas" });
}

void QuestionBlock::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();
	if (VectorHas(Group::PLAYER, groups) && data.edge.y == -1.0f) {
		ExposeContent();
		return;
	}

	if (VectorHas(std::string("Koopas"), groups)) {
		Koopa* koopa = static_cast<Koopa*>(data.who);

		if (koopa->IsSliding())
			ExposeContent();

		return;
	}
}

void QuestionBlock::ExposeContent()
{
	parentScene->QueueFree(this);
	parentScene->AddEntity(content);
	content = nullptr;
}


