#include "Brick.h"
#include "Koopa.h"
#include "Group.h"
#include "Scene.h"
using namespace Entities;
using namespace Utils;

Brick::Brick(LPEntity content, const Utils::Vector2<float>& position)
	: Entity::Entity(
		position,
		"BrickNormal",
		HitboxId::TILE_SIZE_HITBOX,
		{ "Bricks", Group::COLLISION_WALLS, Group::COLLISION_WALLS_TYPE_1 },
		GridType::STATIC_ENTITIES
	),
	content(content)
{
}

void Brick::OnReady() {
	CollisionEngine::Subscribe(this, &Brick::OnCollision, { Group::PLAYER, "Koopas" });
}


void Brick::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();
	if (Contains(Group::PLAYER, groups) && data.edge.y == -1.0f) {
		ExposeContent();
		return;
	}

	if (Contains(std::string("Koopas"), groups)) {
		Koopa* koopa = static_cast<Koopa*>(data.who);
		if (koopa->IsSliding())
			ExposeContent();

		return;
	}
}

void Brick::ExposeContent()
{
	parentScene->QueueFree(this);
	if (content != nullptr)
		parentScene->AddEntity(content);
	content = nullptr;
}
