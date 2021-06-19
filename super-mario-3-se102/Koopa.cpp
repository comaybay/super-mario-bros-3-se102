#include "Koopa.h"
#include "Mario.h"
#include "Goomba.h"
#include "Groups.h"
#include "Game.h"
#include "CollisionHandling.h"
#include "EntityConstants.h"
using namespace Entities;
using namespace Utils;

const float Koopa::WALK_SPEED = 30;
const float Koopa::SHELL_SLIDE_SPEED = 240;
const float Koopa::FRICTION = 2600;

Koopa::Koopa(const std::string& colorCode, const Utils::Vector2<float>& position)
	: Entity(position, colorCode + "KoopaML", "HitboxKoopa", { "Koopas", Groups::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorCode(colorCode),
	state(EntityState<Koopa>(this, &Koopa::MoveAround))
{}

void Koopa::OnReady()
{
	CollisionEngine::Subscribe(this, &Koopa::OnCollision, { Groups::COLLISION_WALLS, Groups::ENEMIES, Groups::PLAYER });
	LPEntity player = parentScene->GetEntitiesByGroup(Groups::PLAYER).front();
	velocity.x = (player->GetPosition().x < position.x) ? -WALK_SPEED : WALK_SPEED;
}

void Koopa::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (VectorHas(Groups::COLLISION_WALLS, groups)) {
		HandleWallCollision(data);
		return;
	}

	if (VectorHas(Groups::PLAYER, groups)) {
		HandlePlayerCollision(data);
		return;
	}

	if (VectorHas(Groups::ENEMIES, groups)) {
		if (state.GetHandler() == &Koopa::ShellIdle)
			return;

		if (state.GetHandler() == &Koopa::MoveAround) {
			CollisionHandling::Slide(this, data);
			if (data.edge.x == 0.0f)
				return;

			velocity.x = WALK_SPEED * data.edge.x;

			std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
			SetAnimation(colorCode + anim);
		}
	}
}

void Koopa::HandlePlayerCollision(const CollisionData& data)
{
	Mario* mario = static_cast<Mario*>(data.who);
	if (state.GetHandler() == &Koopa::MoveAround) {
		if (data.edge.y == 1.0f) {
			mario->SwitchState(&Mario::Bounce);
			SwitchState(&Koopa::ShellIdle);
			velocity.x = 0;
			position.y += Game::TILE_SIZE;
		}
		else
			mario->TakeDamage();

		return;
	}

	if (state.GetHandler() == &Koopa::ShellIdle) {
		if (data.edge.x != 0.0f)
			velocity.x = SHELL_SLIDE_SPEED * data.edge.x;

		else {
			bool isPlayerOnLeftSide = (position.x < mario->GetPosition().x + GetCurrentSpriteDimension().width / 2);
			float direction = isPlayerOnLeftSide ? 1.0f : -1.0f;
			velocity.x = SHELL_SLIDE_SPEED * direction;
		}

		SwitchState(&Koopa::ShellSlide);
		return;
	}

	if (state.GetHandler() == &Koopa::ShellSlide) {
		if (data.edge.x != 0.0f || data.edge.y == -1.0f) {
			mario->TakeDamage();
			return;
		}

		mario->SwitchState(&Mario::Bounce);
		SwitchState(&Koopa::ShellIdle);
		return;
	}
}

void Koopa::HandleWallCollision(const CollisionData& data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (VectorHas(Groups::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y != 0.0f)
			velocity.y = 0;

		if (data.edge.x != 0.0f) {
			float speed = (state.GetHandler() == &Koopa::MoveAround) ? WALK_SPEED : SHELL_SLIDE_SPEED;
			velocity.x = speed * data.edge.x;

			if (state.GetHandler() == &Koopa::MoveAround) {
				std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
				SetAnimation(colorCode + anim);
			}
		}
	}

	if (VectorHas(Groups::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1.0f) {
		CollisionHandling::Slide(this, data);
		velocity.y = 0;
	}
}

void Koopa::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}

void Koopa::SwitchState(EntityState<Koopa>::Handler handler)
{
	state.SetHandler(handler);

	if (handler == &Koopa::ShellIdle) {
		SetAnimation(colorCode + "KoopaSI");
		SetHitbox("HitboxKoopaShell");
	}

	else if (handler == &Koopa::ShellSlide)
		SetAnimation(colorCode + "KoopaSM");
}

void Koopa::MoveAround(float delta)
{
}

void Koopa::ShellIdle(float delta)
{
	if (velocity.x != 0) {
		float moveDirection = Sign(velocity.x);
		velocity.x += FRICTION * delta * -moveDirection;
		if (moveDirection != Sign(velocity.x))
			velocity.x = 0;
	}
}

void Koopa::ShellSlide(float delta)
{
}
