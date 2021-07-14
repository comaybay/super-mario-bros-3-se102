#include "Koopa.h"
#include "Mario.h"
#include "FXBoom.h"
#include "Color.h"
#include "Group.h"
#include "Constants.h"
#include "Scene.h"
#include "Contains.h"
#include "CollisionHandling.h"
#include "EntityConstants.h"
#include "PointUpFactory.h"
#include "EntityUtils.h"

using namespace Entities;
using namespace Utils;

const float Koopa::WALK_SPEED = 29;
const float Koopa::SHELL_SLIDE_SPEED = 200;
const float Koopa::FRICTION = 2600;

Koopa::Koopa(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxKoopa", { "Koopas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorType(colorType),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<Koopa>(this, &Koopa::MoveAround)),
	onGround(false),
	lock(true)
{
}

void Koopa::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Koopa::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYERS });

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -WALK_SPEED : WALK_SPEED;
		SetAnimation(colorCode + ((velocity.x < 0) ? "KoopaML" : "KoopaMR"));
	}
	else {
		SetAnimation(colorCode + "KoopaML");
		velocity.x = -WALK_SPEED;
	}
}

void Koopa::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS, groups)) {
		HandleWallCollision(data);
		return;
	}

	if (Contains(Group::PLAYERS, groups)) {
		HandlePlayerCollision(data);
		return;
	}

	if (Contains(Group::ENEMIES, groups)) {
		if (state.GetState() == &Koopa::ShellIdle)
			return;

		if (state.GetState() == &Koopa::MoveAround) {
			CollisionHandling::Slide(this, data);
			if (data.edge.x == 0.0f)
				return;

			velocity.x = WALK_SPEED * data.edge.x;

			std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
			SetAnimation(colorCode + anim);
			return;
		}

		if (state.GetState() == &Koopa::ShellSlide) {
			if (ContainsAnyOf({ "Goombas", "ParaGoombas", "ParaKoopas" }, groups)) {
				HDirection dir = data.edge.x == 1.0f ? HDirection::LEFT : HDirection::RIGHT;
				dynamic_cast<IKnockedOverable*>(data.who)->GetKnockedOver(dir);
				return;
			}

			if (Contains("Koopas", groups)) {
				Koopa* otherKoopa = dynamic_cast<Koopa*>(data.who);

				if (otherKoopa->IsSliding())
					GetKnockedOver(data.edge.x == 1.0f ? HDirection::RIGHT : HDirection::LEFT);

				otherKoopa->GetKnockedOver(data.edge.x == 1.0f ? HDirection::LEFT : HDirection::RIGHT);
				return;
			}
		}
	}
}

void Koopa::HandlePlayerCollision(const CollisionData& data)
{
	LPMario player = static_cast<LPMario>(data.who);

	if (state.GetState() == &Koopa::MoveAround) {
		if (data.edge.y == 1.0f) {
			player->Bounce();
			SwitchState(&Koopa::ShellIdle);
			velocity.x = 0;
			position.y += Constants::TILE_SIZE;
			parentScene->AddEntity(PointUpFactory::Create(position));
			return;
		}

		if (!player->IsInvincible()) {
			player->TakeDamage();
			velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -WALK_SPEED : WALK_SPEED;
			SetAnimation(colorCode + ((velocity.x < 0) ? "KoopaML" : "KoopaMR"));
			return;
		}
	}

	if (state.GetState() == &Koopa::ShellIdle) {
		if (data.edge.x != 0.0f)
			velocity.x = SHELL_SLIDE_SPEED * data.edge.x;

		else {
			velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? SHELL_SLIDE_SPEED : -SHELL_SLIDE_SPEED;
			parentScene->AddEntity(PointUpFactory::Create(position));
		}

		SwitchState(&Koopa::ShellSlide);
		return;
	}

	if (state.GetState() == &Koopa::ShellSlide) {
		if (data.edge.y == 1.0f) {
			player->Bounce();
			SwitchState(&Koopa::ShellIdle);
			parentScene->AddEntity(PointUpFactory::Create(position));
			return;
		}

		if (!player->IsInvincible()) {
			player->TakeDamage();
			return;
		}

	}
}

void Koopa::HandleWallCollision(const CollisionData& data)
{
	//get position before affected by collision handling
	prevOnGroundPosition = position;

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
			float speed = (state.GetState() == &Koopa::MoveAround) ? WALK_SPEED : SHELL_SLIDE_SPEED;
			velocity.x = speed * data.edge.x;

			if (state.GetState() == &Koopa::MoveAround) {
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
	state.Update(delta);

	if (state.GetState() == &Koopa::ShellSlide)
		velocity.y += EntityConstants::GRAVITY_STRONG * delta;
	else
		velocity.y += EntityConstants::GRAVITY * delta;

	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	onGround = false;
}

bool Entities::Koopa::IsSliding()
{
	return state.GetState() == &Koopa::ShellSlide;
}

void Koopa::SwitchState(EntityState<Koopa>::StateHandler handler)
{
	state.SetState(handler);

	if (handler == &Koopa::ShellIdle) {
		SetAnimation(colorCode + "KoopaSI");
		SetHitbox("HitboxKoopaShell");
	}

	else if (handler == &Koopa::ShellSlide)
		SetAnimation(colorCode + "KoopaSM");
}

void Koopa::MoveAround(float delta)
{
	if (colorType == Color::GREEN || onGround || lock)
		return;

	lock = true;
	//revert to previous position (before falling) and change direction
	velocity = { -velocity.x , 0 };
	position = prevOnGroundPosition;

	std::string anim = (Sign(velocity.x) < 0) ? "KoopaML" : "KoopaMR";
	SetAnimation(colorCode + anim);
}

void Koopa::ShellIdle(float delta)
{
	if (velocity.x == 0)
		return;

	int moveDirection = Sign(velocity.x);
	velocity.x += FRICTION * delta * -moveDirection;
	if (moveDirection != Sign(velocity.x))
		velocity.x = 0;
}

void Koopa::ShellSlide(float delta)
{
}

void Koopa::KnockedOver(float delta)
{
	knockedOverMovement->Update(delta);
	if (knockedOverMovement->Finished())
		parentScene->QueueFree(this);
}

void Koopa::GetKnockedOver(HDirection direction)
{
	SetDetectable(false);
	SetAnimation(colorCode + "KoopaKO");

	knockedOverMovement = new MovementKnockedOver(this, direction);
	state.SetState(&Koopa::KnockedOver);

	parentScene->AddEntity(new FXBoom(position));
	parentScene->AddEntity(PointUpFactory::Create(position));
}
