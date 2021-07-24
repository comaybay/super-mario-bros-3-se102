#include "Koopa.h"
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
#include "Game.h"

using namespace Entities;
using namespace Utils;

const float Koopa::WALK_SPEED = 29;
const float Koopa::SHELL_SLIDE_SPEED = 200;
const float Koopa::FRICTION = 2600;

Koopa::~Koopa()
{
	delete knockedOverMovement;
}

Koopa::Koopa(const std::string& colorType, const Utils::Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxKoopa", { "Koopas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorType(colorType),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<Koopa>(this, &Koopa::MoveAround)),
	knockedOverMovement(nullptr),
	onGround(false),
	lock(true),
	holder(nullptr)
{
}

void Koopa::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &Koopa::OnCollision, { Group::COLLISION_WALLS, Group::BLOCKS, Group::ENEMIES, Group::PLAYERS });

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
	const EntityGroups& groups = data.who->GetEntityGroups();

	if (state.GetState() != &Koopa::Hold) {
		if (Contains(Group::BLOCKS, groups) && IsSliding() && data.edge.x != 0) {
			if (IKnockedOverable* entity = dynamic_cast<IKnockedOverable*>(data.who)) {
				HDirection dir = data.edge.x == 1.0f ? HDirection::LEFT : HDirection::RIGHT;
				entity->GetKnockedOver(dir);
			};
		}

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
				if (data.edge.x == 0)
					return;

				velocity.x = WALK_SPEED * data.edge.x;

				std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
				SetAnimation(colorCode + anim);
				return;
			}

			if (IsSliding()) {
				if (Contains("Koopas", groups)) {
					LPKoopa otherKoopa = static_cast<LPKoopa>(data.who);

					if (otherKoopa->IsSliding())
						GetKnockedOver(data.edge.x == 1.0f ? HDirection::RIGHT : HDirection::LEFT);

					otherKoopa->GetKnockedOver(data.edge.x == 1.0f ? HDirection::LEFT : HDirection::RIGHT);
					return;
				}

				//if entity implements IKnockedOverable
				if (IKnockedOverable* entity = dynamic_cast<IKnockedOverable*>(data.who)) {
					HDirection dir = data.edge.x == 1.0f ? HDirection::LEFT : HDirection::RIGHT;
					entity->GetKnockedOver(dir);
				};

				return;
			}
		}
	}

	if (state.GetState() == &Koopa::Hold && Contains(Group::ENEMIES, groups)) {
		//if entity implements IKnockedOverable
		if (IKnockedOverable* entity = dynamic_cast<IKnockedOverable*>(data.who)) {
			HDirection dir = data.edge.x == 1.0f ? HDirection::LEFT : HDirection::RIGHT;
			entity->GetKnockedOver(dir);
		};

		holder->GetReleaseHoldEvent().Unsubscribe(this, &Koopa::OnPlayerReleaseHold);
		holder->ReleaseHold();
		holder = nullptr;
		GetKnockedOver(data.edge.x == 1.0f ? HDirection::RIGHT : HDirection::LEFT);
		return;
	}


}

void Koopa::OnPlayerReleaseHold(LPEntity player)
{
	SwitchState(&Koopa::ShellSlide);
	velocity.y = -SHELL_SLIDE_SPEED / 2.0f;
	velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? SHELL_SLIDE_SPEED : -SHELL_SLIDE_SPEED;
	holder = nullptr;
}

void Koopa::HandlePlayerCollision(const CollisionData& data)
{
	LPMario player = static_cast<LPMario>(data.who);

	if (state.GetState() == &Koopa::MoveAround) {
		if (data.edge.y == 1) {
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
		bool pressedHold = Game::IsKeyDown(DIK_A);

		if (data.edge.x != 0 && !pressedHold) {
			velocity.x = SHELL_SLIDE_SPEED * data.edge.x;
			SwitchState(&Koopa::ShellSlide);
			return;
		}

		if (data.edge.x != 0 && pressedHold) {
			player->Hold(this);
			player->GetReleaseHoldEvent().Subscribe(this, &Koopa::OnPlayerReleaseHold);
			holder = player;
			SwitchState(&Koopa::Hold);
			return;
		}

		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? SHELL_SLIDE_SPEED : -SHELL_SLIDE_SPEED;
		parentScene->AddEntity(PointUpFactory::Create(position));
		SwitchState(&Koopa::ShellSlide);
		return;
	}

	if (state.GetState() == &Koopa::ShellSlide) {
		if (data.edge.y == 1) {
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

	const EntityGroups& groups = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS_TYPE_1, groups)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y != 0) {
			velocity.y = 0;
			onGround = true;
			lock = false;
			return;
		}

		if (data.edge.x != 0) {
			float speed = (state.GetState() == &Koopa::MoveAround) ? WALK_SPEED : SHELL_SLIDE_SPEED;
			velocity.x = speed * data.edge.x;

			if (state.GetState() == &Koopa::MoveAround) {
				std::string anim = (data.edge.x < 0) ? "KoopaML" : "KoopaMR";
				SetAnimation(colorCode + anim);
			}
		}
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, groups) && data.edge.y == -1) {
		CollisionHandling::Slide(this, data);
		velocity.y = 0;
		onGround = true;
		lock = false;
	}
}

void Koopa::Update(float delta)
{
	//not allow update position by velocity when hold
	if (holder == nullptr)
		Entity::Update(delta);

	state.Update(delta);
	onGround = false;
}

bool Koopa::IsSliding()
{
	return state.GetState() == &Koopa::ShellSlide;
}

void Koopa::SwitchState(EntityState<Koopa>::StateHandler handler)
{
	state.SetState(handler);

	if (handler == &Koopa::KnockedOver) {
		SetAnimation(colorCode + "KoopaKO");
		return;
	}

	if (handler == &Koopa::ShellIdle) {
		SetAnimation(colorCode + "KoopaSI");
		SetHitbox("HitboxKoopaShell");
		return;
	}

	if (handler == &Koopa::ShellSlide) {
		SetAnimation(colorCode + "KoopaSM");
		return;
	}
}

void Koopa::MoveAround(float delta)
{
	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);

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
	velocity.y = min(velocity.y + EntityConstants::GRAVITY * delta, EntityConstants::MAX_FALL_SPEED);

	if (velocity.x == 0)
		return;

	int moveDirection = Sign(velocity.x);
	velocity.x += FRICTION * delta * -moveDirection;
	if (moveDirection != Sign(velocity.x))
		velocity.x = 0;
}

void Koopa::ShellSlide(float delta)
{
	velocity.y = min(velocity.y + EntityConstants::GRAVITY_STRONG * delta, EntityConstants::MAX_FALL_SPEED);
}

void Koopa::KnockedOver(float delta)
{
	knockedOverMovement->Update(delta);
	if (knockedOverMovement->Finished())
		parentScene->QueueFree(this);
}

void Koopa::Hold(float _)
{
}

void Koopa::GetKnockedOver(HDirection direction)
{
	SetDetectable(false);
	CollisionEngine::Unsubscribe(this, &Koopa::OnCollision);

	delete knockedOverMovement;
	knockedOverMovement = new MovementKnockedOver(this, direction);
	SwitchState(&Koopa::KnockedOver);

	parentScene->AddEntity(new FXBoom(position));
	parentScene->AddEntity(PointUpFactory::Create(position));
}
