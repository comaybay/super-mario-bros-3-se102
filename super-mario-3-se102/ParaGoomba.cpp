#include "ParaGoomba.h"
#include "Goomba.h"
#include "CollisionHandling.h"
#include "Group.h"
#include "EntityConstants.h"
#include "Scene.h"
#include "Mario.h"
#include "FXBoom.h"
#include "PointUpFactory.h"
#include "Wing.h"
#include "Contains.h"
#include "EntityUtils.h"
using namespace Entities;
using namespace Utils;

const float ParaGoomba::TIME_TILL_PREPARE = 0.75f;
const float ParaGoomba::PREPARE_JUMP_SPEED = 100;
const int ParaGoomba::NUM_OF_PREPARE_JUMPS = 3;
const float ParaGoomba::JUMP_SPEED = 240;
const float ParaGoomba::FALL_SPEED = EntityConstants::GRAVITY / 1.2f;
const float ParaGoomba::JUMP_FLAP_ANIM_SPEED = 3;

ParaGoomba::ParaGoomba(const std::string& colorType, const Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxGoomba", { "ParaGoombas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorType(colorType),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<ParaGoomba>(this, &ParaGoomba::MoveAround)),
	onGround(false),
	jumpCount(0),
	time(0),
	wingLeft(Wing(this, WingDirection::LEFT, { -2, -10 })),
	wingRight(Wing(this, WingDirection::RIGHT, { 10, -10 }))
{
	SetAnimation(colorCode + "GoombaM");
	wingLeft.FlapDown();
	wingRight.FlapDown();
}

void ParaGoomba::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &ParaGoomba::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYERS });

	if (!parentScene->IsEntityGroupEmpty(Group::PLAYERS)) {
		LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -Goomba::WALK_SPEED : Goomba::WALK_SPEED;
	}
	else
		velocity.x = -Goomba::WALK_SPEED;
}

ParaGoomba::~ParaGoomba()
{
}

void ParaGoomba::Update(float delta)
{
	Entity::Update(delta);

	state.Update(delta);
	velocity.y += FALL_SPEED * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);

	wingLeft.Update(delta);
	wingRight.Update(delta);

	onGround = false;
}

void ParaGoomba::Render() {
	wingLeft.Render();
	wingRight.Render();
	Entity::Render();
}

void ParaGoomba::MoveAround(float delta) {
	time += delta;

	if (time >= TIME_TILL_PREPARE) {
		time = 0;
		wingLeft.AutoFlap();
		wingRight.AutoFlap();
		state.SetState(&ParaGoomba::PrepareToJump);

		if (parentScene->IsEntityGroupEmpty(Group::PLAYERS))
			return;

		LPEntity player = parentScene->GetEntityOfGroup(Group::PLAYERS);
		velocity.x = EntityUtils::IsOnLeftSideOf(this, player) ? -Goomba::WALK_SPEED : Goomba::WALK_SPEED;
	}
}

void ParaGoomba::PrepareToJump(float delta) {
	if (!onGround)
		return;

	jumpCount++;

	if (jumpCount <= NUM_OF_PREPARE_JUMPS)
		velocity.y -= PREPARE_JUMP_SPEED;
	else
	{
		jumpCount = 0;
		velocity.y -= JUMP_SPEED;

		wingLeft.AutoFlap();
		wingRight.AutoFlap();
		wingLeft.SetFlapSpeed(JUMP_FLAP_ANIM_SPEED);
		wingRight.SetFlapSpeed(JUMP_FLAP_ANIM_SPEED);

		state.SetState(&ParaGoomba::Jump);
	}
}

void ParaGoomba::Jump(float delta) {
	if (velocity.y >= 0) {
		wingLeft.SetFlapSpeed(1.0f);
		wingRight.SetFlapSpeed(1.0f);
		state.SetState(&ParaGoomba::Fall);
	}
}

void ParaGoomba::Fall(float delta) {
	if (onGround) {
		wingLeft.FlapDown();
		wingRight.FlapDown();

		state.SetState(&ParaGoomba::MoveAround);
		return;
	}
}

void ParaGoomba::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYERS, groups)) {
		LPMario player = static_cast<LPMario>(data.who);
		if (data.edge.y == 1.0f) {
			player->Bounce();
			parentScene->AddEntity(new Entities::Goomba(colorType, position));
			parentScene->AddEntity(PointUpFactory::Create(position));
			parentScene->QueueFree(this);
			return;
		}

		if (!player->IsInvincible()) {
			player->TakeDamage();
			return;
		}
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, groups)) {
		if (data.edge.y == -1.0f) {
			CollisionHandling::Slide(this, data);
			onGround = true;
		}

		return;
	}

	CollisionHandling::Slide(this, data);

	if (data.edge.y != 0.0f)
		velocity.y = 0;

	if (data.edge.y == -1.0f)
		onGround = true;

	else if (data.edge.x != 0.0f)
		velocity.x = Goomba::WALK_SPEED * data.edge.x;
}

void ParaGoomba::GetKnockedOver(HDirection direction)
{
	Goomba* goomba = new Entities::Goomba(colorType, position);
	goomba->GetKnockedOver(direction);
	parentScene->AddEntity(goomba);
	parentScene->QueueFree(this);
}