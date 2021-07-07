#include "Mushroom.h"
#include "Scene.h"
#include "Group.h"
#include "Mario.h"
#include "Constants.h"
#include "EntityConstants.h"
#include "CollisionHandling.h"
#include "PointUp.h"
using namespace Entities;
using namespace Utils;

const float Mushroom::WAIT_TIME = 0.3f;
const float Mushroom::RISE_SPEED = 20;
const float Mushroom::MOVE_SPEED = 50;
const float Mushroom::FALL_SPEED = EntityConstants::GRAVITY_STRONG / 1.25f;

Mushroom::Mushroom(const Vector2<float>& position)
	: Entity(position, AnimationId::NONE, HitboxId::NONE, { "Mushrooms", Group::ITEMS }, GridType::MOVABLE_ENTITIES),
	state(EntityState<Mushroom>(this, &Mushroom::Wait)),
	riseDestY(position.y + 1), //+1 because of hitbox
	time(0)
{
	SetRenderedBeforeWorld(true);
	this->position.y += Constants::TILE_SIZE;
}

void Mushroom::Update(float delta) {
	Entity::Update(delta);
	state.Handle(delta);
}

void Mushroom::OnReady()
{
	CollisionEngine::Subscribe(this, &Mushroom::OnCollision, { Group::PLAYER, Group::COLLISION_WALLS });
}


void Mushroom::Wait(float delta)
{
	time += delta;
	if (time >= WAIT_TIME) {
		velocity.y = -RISE_SPEED;
		SetAnimation("Mushroom");
		SetHitbox("HitboxMushroom");
		state.SetHandler(&Mushroom::RiseUp);
	}
}

void Mushroom::RiseUp(float delta)
{
	if (position.y > riseDestY)
		return;

	velocity.y = 0;
	position.y = riseDestY;

	const std::list<LPEntity>& playerGroup = parentScene->GetEntitiesByGroup(Group::PLAYER);
	if (!playerGroup.empty()) {
		LPMario player = static_cast<LPMario>(playerGroup.front());
		bool moveRight = player->GetPosition().x + player->GetCurrentSpriteDimension().width / 2.0f < position.x + GetCurrentSpriteDimension().width / 2.0f;
		velocity.x = moveRight ? MOVE_SPEED : -MOVE_SPEED;
	}
	else
		velocity.x = -MOVE_SPEED;

	SetRenderedBeforeWorld(false);
	state.SetHandler(&Mushroom::Move);
}

void Mushroom::Move(float delta)
{
	velocity.y += FALL_SPEED * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}


void Mushroom::OnCollision(CollisionData data) {
	const std::vector<std::string>& group = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS, group))
		HandleWallCollision(data);

	if (Contains(Group::PLAYER, group))
		HandlePlayerCollision(data);
}

void Mushroom::HandleWallCollision(const CollisionData& data) {
	const std::vector<std::string>& group = data.who->GetEntityGroups();

	if (Contains(Group::COLLISION_WALLS_TYPE_1, group)) {
		CollisionHandling::Slide(this, data);

		if (data.edge.y == -1.0f)
			velocity.y = 0;

		else if (data.edge.x != 0.0f)
			velocity.x = MOVE_SPEED * data.edge.x;

		return;
	}

	if (Contains(Group::COLLISION_WALLS_TYPE_2, group) && data.edge.y == -1.0f) {
		CollisionHandling::Slide(this, data);
		velocity.y = 0;
		return;
	}
}

void Mushroom::HandlePlayerCollision(const CollisionData& data) {
	//TODO: complete implementation
	parentScene->QueueFree(this);
	Vector2<float> pointPos = { position.x, position.y - Constants::TILE_SIZE };
	parentScene->AddEntity(new PointUp(pointPos, PointUp::Type::ONE_THOUSAND_POINTS));
	return;
}