#include "Koopa.h"
#include "Color.h"
#include "Mario.h"
#include "Goomba.h"
#include "ParaGoomba.h"
#include "Group.h"
#include "Game.h"
#include "Contains.h"
#include "CollisionHandling.h"
#include "EntityConstants.h"
#include "PointUpFactory.h"
using namespace Entities;
using namespace Utils;

const float Koopa::WALK_SPEED = 30;
const float Koopa::SHELL_SLIDE_SPEED = 240;
const float Koopa::FRICTION = 2600;

Koopa::Koopa(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxKoopa", { "Koopas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<Koopa>(this, &Koopa::MoveAround)),
	onGround(false),
	lock(true)
{
}

void Koopa::OnReady()
{
	CollisionEngine::Subscribe(this, &Koopa::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYER });
	const std::list<LPEntity>& playerGroup = parentScene->GetEntitiesByGroup(Group::PLAYER);

	if (playerGroup.empty()) {
		SetAnimation(colorCode + "KoopaML");
		velocity.x = -WALK_SPEED;
		return;
	}

	LPEntity player = playerGroup.front();
	velocity.x = (player->GetPosition().x < position.x) ? -WALK_SPEED : WALK_SPEED;
	SetAnimation(colorCode + ((velocity.x < 0) ? "KoopaML" : "KoopaMR"));
}

void Koopa::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS, groups)) {
		HandleWallCollision(data);
		return;
	}

	if (Contains(Group::PLAYER, groups)) {
		HandlePlayerCollision(data);
		return;
	}

	if (Contains(Group::ENEMIES, groups)) {
		if (state.GetHandler() == &Koopa::ShellIdle)
			return;

		if (state.GetHandler() == &Koopa::MoveAround) {
			CollisionHandling::Slide(this, data);
			if (data.edge.x == 0.0f)
				return;

			velocity.x = WALK_SPEED * data.edge.x;

			std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
			SetAnimation(colorCode + anim);
			return;
		}

		if (state.GetHandler() == &Koopa::ShellSlide) {
			if (Contains(std::string("Goombas"), data.who->GetEntityGroups())) {
				static_cast<Goomba*>(data.who)->KnockOver(-data.edge.x);
				return;
			}

			if (Contains(std::string("ParaGoombas"), data.who->GetEntityGroups())) {
				static_cast<ParaGoomba*>(data.who)->KnockOver(-data.edge.x);
				return;
			}
		}
	}
}

void Koopa::HandlePlayerCollision(const CollisionData& data)
{
	Mario* mario = static_cast<Mario*>(data.who);
	if (state.GetHandler() == &Koopa::MoveAround) {
		if (data.edge.y == 1.0f) {
			mario->Bounce();
			SwitchState(&Koopa::ShellIdle);
			velocity.x = 0;
			position.y += Game::TILE_SIZE;
			parentScene->AddEntity(PointUpFactory::Create(position));
		}
		else {
			mario->TakeDamage();
			velocity.x = (mario->GetPosition().x < position.x) ? -WALK_SPEED : WALK_SPEED;
			SetAnimation(colorCode + ((velocity.x < 0) ? "KoopaML" : "KoopaMR"));
		}

		return;
	}

	if (state.GetHandler() == &Koopa::ShellIdle) {
		if (data.edge.x != 0.0f)
			velocity.x = SHELL_SLIDE_SPEED * data.edge.x;

		else {
			bool isPlayerOnLeftSide = (position.x < mario->GetPosition().x + GetCurrentSpriteDimension().width / 2);
			float direction = isPlayerOnLeftSide ? 1.0f : -1.0f;
			velocity.x = SHELL_SLIDE_SPEED * direction;
			parentScene->AddEntity(PointUpFactory::Create(position));
		}

		SwitchState(&Koopa::ShellSlide);
		return;
	}

	if (state.GetHandler() == &Koopa::ShellSlide) {
		if (data.edge.x != 0.0f || data.edge.y == -1.0f) {
			mario->TakeDamage();
			return;
		}

		mario->Bounce();
		SwitchState(&Koopa::ShellIdle);
		parentScene->AddEntity(PointUpFactory::Create(position));
		return;
	}
}

void Koopa::HandleWallCollision(const CollisionData& data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y != 0.0f) {
			velocity.y = 0;
			onGround = true;
			lock = false;
			return;
		}

		if (data.edge.x != 0.0f) {
			float speed = (state.GetHandler() == &Koopa::MoveAround) ? WALK_SPEED : SHELL_SLIDE_SPEED;
			velocity.x = speed * data.edge.x;

			if (state.GetHandler() == &Koopa::MoveAround) {
				std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
				SetAnimation(colorCode + anim);
			}
		}
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1.0f) {
		CollisionHandling::Slide(this, data);
		velocity.y = 0;
		onGround = true;
		lock = false;
	}
}

void Koopa::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);
	prevPosition = position;

	velocity.y += EntityConstants::GRAVITY * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
	onGround = false;
}

bool Entities::Koopa::IsSliding()
{
	return state.GetHandler() == &Koopa::ShellSlide;
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
	if (!onGround && !lock) {
		lock = true;
		//revert to previous position (before falling) and change direction
		velocity = { -velocity.x , 0 };
		position = prevPosition + velocity * delta;
		std::string anim = (Sign(velocity.x) < 0) ? "KoopaML" : "KoopaMR";
		SetAnimation(colorCode + anim);
	}
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
