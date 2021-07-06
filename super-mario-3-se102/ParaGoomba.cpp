#include "ParaGoomba.h"
#include "Goomba.h"
#include "CollisionHandling.h"
#include "Group.h"
#include "EntityConstants.h"
#include "Scene.h"
#include "Mario.h"
#include "BoomFX.h"
#include "PointUpFactory.h"
#include "Wing.h"
#include "Contains.h"
using namespace Entities;
using namespace Utils;

const float ParaGoomba::TIME_TILL_PREPARE = 0.75f;
const float ParaGoomba::PREPARE_JUMP_SPEED = 100;
const int ParaGoomba::NUM_OF_PREPARE_JUMPS = 3;
const float ParaGoomba::JUMP_SPEED = 250;
const float ParaGoomba::JUMP_FLAP_ANIM_SPEED = 3;

ParaGoomba::ParaGoomba(const std::string& colorType, const Vector2<float>& position)
	: Entity(position, AnimationId::NONE, "HitboxGoomba", { "ParaGoombas", Group::ENEMIES }, GridType::MOVABLE_ENTITIES),
	colorType(colorType),
	colorCode(Color::ToColorCode(colorType)),
	state(EntityState<ParaGoomba>(this, &ParaGoomba::MoveAround)),
	onGround(false),
	jumpCount(0),
	time(0),
	wingLeft(Wing(this, Wing::Direction::LEFT, Vector2<float>(-2, -10))),
	wingRight(Wing(this, Wing::Direction::RIGHT, Vector2<float>(10, -10)))
{
	SetAnimation(colorCode + "GoombaM");
	wingLeft.FlapDown();
	wingRight.FlapDown();
}

void ParaGoomba::OnReady()
{
	Entity::OnReady();
	CollisionEngine::Subscribe(this, &ParaGoomba::OnCollision, { Group::COLLISION_WALLS, Group::ENEMIES, Group::PLAYER });
	const std::list<LPEntity>& playerGroup = parentScene->GetEntitiesByGroup(Group::PLAYER);

	if (playerGroup.empty()) {
		velocity.x = -Goomba::WALK_SPEED;
		return;
	}

	LPEntity player = playerGroup.front();
	velocity.x = (player->GetPosition().x < position.x) ? -Goomba::WALK_SPEED : Goomba::WALK_SPEED;
}

ParaGoomba::~ParaGoomba()
{
	if (state.GetHandler() == &ParaGoomba::StompedOn)
		parentScene->AddEntity(new Entities::Goomba(colorType, position));
}

void ParaGoomba::Update(float delta)
{
	Entity::Update(delta);

	state.Handle(delta);
	velocity.y += EntityConstants::GRAVITY * delta;
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
		state.SetHandler(&ParaGoomba::PrepareToJump);

		std::list<LPEntity> playerGroup = parentScene->GetEntitiesByGroup(Group::PLAYER);
		if (playerGroup.empty())
			return;

		LPEntity player = playerGroup.front();
		velocity.x = (player->GetPosition().x < position.x) ? -Goomba::WALK_SPEED : Goomba::WALK_SPEED;
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

		state.SetHandler(&ParaGoomba::Jump);
	}
}

void ParaGoomba::Jump(float delta) {
	if (velocity.y >= 0) {
		wingLeft.SetFlapSpeed(1.0f);
		wingRight.SetFlapSpeed(1.0f);
		state.SetHandler(&ParaGoomba::Fall);
	}
}

void ParaGoomba::Fall(float delta) {
	if (onGround) {
		wingLeft.FlapDown();
		wingRight.FlapDown();

		state.SetHandler(&ParaGoomba::MoveAround);
		return;
	}
}

void ParaGoomba::StompedOn(float delta) {
	time += delta;

	if (time >= 0.25f)
		parentScene->QueueFree(this);
}

void ParaGoomba::OnCollision(CollisionData data)
{
	const std::vector<std::string>& groups = data.who->GetEntityGroups();

	if (Contains(Group::PLAYER, groups)) {
		Mario* mario = static_cast<Mario*>(data.who);

		if (data.edge.y == 1.0f) {
			mario->Bounce();
			state.SetHandler(&ParaGoomba::StompedOn);
			parentScene->AddEntity(PointUpFactory::Create(position));
			parentScene->QueueFree(this);
			return;
		}

		mario->TakeDamage();
		velocity.x = (position.x < data.who->GetPosition().x) ? Goomba::WALK_SPEED : -Goomba::WALK_SPEED;
		return;
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

void ParaGoomba::KnockOver(float horizontalDirection)
{
	Goomba* goomba = new Entities::Goomba(colorType, position);
	parentScene->AddEntity(goomba);
	goomba->KnockOver(horizontalDirection);
	parentScene->QueueFree(this);
}