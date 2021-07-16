#include "FXBrickBreak.h"
#include "Scene.h"
#include "EntityConstants.h"
#include "Group.h"
using namespace Entities;
using namespace Utils;

FXBrickBreak::BrickBreakParticle::BrickBreakParticle(const Vector2<float>& position, const Vector2<float>& velocity)
	: Entity::Entity(position, "FXBrickBreakParticle", HitboxId::NONE, Group::EFFECTS, GridType::MOVABLE_ENTITIES)
{
	this->velocity = velocity;
}

void FXBrickBreak::BrickBreakParticle::Update(float delta) {
	Entity::Update(delta);

	velocity.y += EntityConstants::GRAVITY * 1.5f * delta;
	velocity.y = min(velocity.y, EntityConstants::MAX_FALL_SPEED);
}

const float FXBrickBreak::UP_SPEED = 380;
const float FXBrickBreak::X_SPEED = 70;
const float FXBrickBreak::MARGIN = 8;
const float FXBrickBreak::EXIST_DURATION = 2;

FXBrickBreak::FXBrickBreak(const Vector2<float>& position)
	: Entity::Entity(position, Group::EFFECTS, GridType::NONE),
	time(0),
	particles({ {
		new BrickBreakParticle({ position.x , position.y }, { -X_SPEED, -UP_SPEED, }),
		new BrickBreakParticle({ position.x , position.y + MARGIN }, { -X_SPEED, -UP_SPEED / 1.5f }),
		new BrickBreakParticle({ position.x + MARGIN , position.y }, { X_SPEED, -UP_SPEED }),
		new BrickBreakParticle({ position.x + MARGIN , position.y + MARGIN }, { X_SPEED, -UP_SPEED / 1.5f })
		} })
{
}

FXBrickBreak::~FXBrickBreak()
{
	for (LPEntity particle : particles)
		delete particle;
}

void FXBrickBreak::Update(float delta)
{
	Entity::Update(delta);

	for (LPEntity& particle : particles) {
		particle->Update(delta);
		particle->PostUpdate();
	}

	time += delta;
	if (time >= EXIST_DURATION)
		parentScene->QueueFree(this);
}

void FXBrickBreak::Render()
{
	Entity::Render();

	for (LPEntity& particle : particles)
		particle->Render();
}


