#include "Brick.h"
#include "EmptyBlock.h"
#include "Koopa.h"
#include "Group.h"
#include "Scene.h"
#include "Mario.h"
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
	content(content),
	state(EntityState<Brick>(this, &Brick::Idle)),
	blockHitMovement(this, position)
{
}

void Brick::OnReady() {
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Brick::OnCollision, { Group::PLAYER, "Koopas" });
}

void Brick::Update(float delta)
{
	Entity::Update(delta);
	state.Handle(delta);
}

void Brick::Idle(float delta) {
}

void Brick::Hit(float delta) {
	bool done = blockHitMovement.Update(delta);
	if (done) {
		state.SetHandler(&Brick::Idle);
		blockHitMovement.Reset();
	}
}


void Brick::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();
	if (Contains(Group::PLAYER, groups) && data.edge.y == -1.0f) {
		LPMario mario = static_cast<LPMario>(data.who);
		if (mario->GetPowerLevel() == PlayerPowerLevel::BIG)
			ExposeContent();
		else
			state.SetHandler(&Brick::Hit);

		return;
	}

	if (Contains(std::string("Koopas"), groups)) {
		Koopa* koopa = static_cast<Koopa*>(data.who);
		if (data.edge.x != 0 && koopa->IsSliding())
			ExposeContent();

		return;
	}
}

void Brick::ExposeContent()
{
	if (content != nullptr) {
		parentScene->AddEntity(content);
		parentScene->AddEntity(new EmptyBlock(position));
		content = nullptr;
	}

	parentScene->QueueFree(this);
}
