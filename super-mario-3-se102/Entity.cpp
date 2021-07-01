#include "Entity.h"
#include "EntityManager.h"
#include "HitboxManager.h"
#include "AnimationManager.h"
#include "Event.h"
#include "Scene.h"
#include "Group.h"

using namespace Utils;

Entity::Entity(
	const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
	const std::vector<std::string>& entityGroups, GridType gridType, bool isRenderedBeforeWorld) :
	position(position),
	animation(AnimationManager::GetNew(initialAnimation)),
	hitbox(HitboxManager::Get(hitbox)),
	groups(entityGroups),
	gridType(gridType),
	isRenderedBeforeWorld(isRenderedBeforeWorld)
{
	Init();
}

Entity::Entity(
	const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
	const std::vector<std::string>& entityGroups, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(initialAnimation)),
	hitbox(HitboxManager::Get(hitbox)),
	groups(entityGroups),
	gridType(gridType),
	isRenderedBeforeWorld(false)
{
	Init();
}

Entity::Entity(
	const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
	const std::string& entityGroup, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(initialAnimation)),
	hitbox(HitboxManager::Get(hitbox)),
	groups(std::vector<std::string> {entityGroup}),
	gridType(gridType),
	isRenderedBeforeWorld(false)
{
	Init();
}

Entity::Entity(const Utils::Vector2<float>& position, const std::vector<std::string>& entityGroups, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(AnimationId::NONE)),
	hitbox(HitboxManager::Get(HitboxId::NONE)),
	groups(entityGroups),
	gridType(gridType),
	isRenderedBeforeWorld(false)
{
	Init();
}

Entity::Entity(const Utils::Vector2<float>& position, const std::string& entityGroup, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(AnimationId::NONE)),
	hitbox(HitboxManager::Get(HitboxId::NONE)),
	groups(std::vector<std::string> {entityGroup}),
	gridType(gridType),
	isRenderedBeforeWorld(false)
{
	Init();
}

void Entity::Init()
{
	groups.push_back(Group::ALL);
	destroyEvent = new Event<LPEntity>();
	id = std::to_string(reinterpret_cast<intptr_t>(this));
	enabledForCollisionDetection = true;
}

bool Entity::_IsEnabledForCollisionDetection()
{
	return enabledForCollisionDetection;
}

Entity::~Entity() {
	destroyEvent->Notify(this);
	delete destroyEvent;
}

LPScene Entity::GetParentScene()
{
	return parentScene;
}

void Entity::SetAnimation(std::string id)
{
	if (id != animation->GetId()) {
		delete animation;
		animation = AnimationManager::GetNew(id);
	}
}

void Entity::SetHitbox(std::string id)
{
	hitbox = HitboxManager::Get(id);
}

Vector2<float> Entity::GetPosition()
{
	return position;
}

void Entity::SetPosition(const Vector2<float>& position)
{
	this->position = position;
}

void Entity::SetVelocity(const Vector2<float>& velocity)
{
	this->velocity = velocity;
}

void Entity::SetEnabledForCollisionDetection(bool enabled)
{
	enabledForCollisionDetection = enabled;
}

const std::string& Entity::GetId()
{
	return id;
}

GridType Entity::GetGridType() {
	return gridType;
}

LPConstHitbox Entity::GetHitbox()
{
	return hitbox;
}

SpriteBox Entity::GetSpriteBox() {
	return animation->GetCurrentSpriteBox();
}

//do nothing by default
void Entity::OnReady() {}

Utils::Dimension Entity::GetCurrentSpriteDimension()
{
	RECT rect1 = GetSpriteBox().rect;
	return Utils::Dimension(rect1.right - rect1.left, rect1.bottom - rect1.top);
}

void Entity::Update(float delta)
{
	position += remainingVelocity * delta;
	animation->Update(delta);
}

//process stuff for collision detection/handling
void Entity::PostUpdate()
{
	remainingVelocity = velocity;
}

void Entity::Render()
{
	animation->Render(position);
}

Event<LPEntity>& Entity::GetDestroyEvent()
{
	return *destroyEvent;
}

bool Entity::_IsRenderedBeforeWorld()
{
	return isRenderedBeforeWorld;
}

const std::vector<std::string>& Entity::GetEntityGroups()
{
	return groups;
}

void Entity::_SetParentScene(LPScene scene) {
	this->parentScene = scene;
}

Utils::Vector2<float> Entity::_GetRemainingVelocity()
{
	return remainingVelocity;
}
void Entity::_SetRemainingVelocity(Utils::Vector2<float> velocity)
{
	remainingVelocity = velocity;
}
