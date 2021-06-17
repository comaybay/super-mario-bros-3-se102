#include "Koopa.h"
#include "Groups.h"
#include "Game.h"
#include "CollisionHandling.h"
#include "EntityConstants.h"
using namespace Entities;
using namespace Utils;

const Vector2<float> Koopa::SPEED(30, 0);

Koopa::Koopa(const std::string& colorCode, const Utils::Vector2<float>& position)
	: Entity(position, colorCode + "KoopaML", "HitboxKoopa", { "Koopas", Groups::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorCode(colorCode),
	state(EntityState<Koopa>(this, &Koopa::MoveAround))
{
}

void Koopa::OnReady()
{
	CollisionEngine::Subscribe(this, &Koopa::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES, Groups::PLAYER });
	LPEntity player = parentScene->GetEntitiesByGroup(Groups::PLAYER).front();
	velocity = (player->GetPosition().x < position.x) ? -SPEED : SPEED;
}

void Koopa::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (VectorHas(Groups::COLLISION_WALLS_TYPE_2, groups)) {
		if (data.edge.y == -1.0f)
			CollisionHandling::Slide(this, data);

		return;
	}

	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f)
		velocity.y = 0;

	else if (data.edge.x != 0.0f) {
		velocity.x = SPEED.x * data.edge.x;

		std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
		SetAnimation(colorCode + anim);
	}
}


void Koopa::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);
}

void Koopa::MoveAround(float delta) {
	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}
