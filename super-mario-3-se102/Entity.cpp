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
	const EntityGroups& entityGroups, GridType gridType, bool isRenderedBeforeForeground) :
	position(position),
	animation(AnimationManager::GetNew(initialAnimation)),
	hitbox(HitboxManager::Get(hitbox)),
	groups(entityGroups),
	gridType(gridType),
	isRenderedBeforeForeground(isRenderedBeforeForeground)
{
	Init();
}

Entity::Entity(
	const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
	const EntityGroups& entityGroups, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(initialAnimation)),
	hitbox(HitboxManager::Get(hitbox)),
	groups(entityGroups),
	gridType(gridType),
	isRenderedBeforeForeground(false)
{
	Init();
}

Entity::Entity(
	const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
	const std::string& entityGroup, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(initialAnimation)),
	hitbox(HitboxManager::Get(hitbox)),
	groups(EntityGroups{ entityGroup }),
	gridType(gridType),
	isRenderedBeforeForeground(false)
{
	Init();
}

Entity::Entity(const Utils::Vector2<float>& position, const EntityGroups& entityGroups, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(AnimationId::NONE)),
	hitbox(HitboxManager::Get(HitboxId::NONE)),
	groups(entityGroups),
	gridType(gridType),
	isRenderedBeforeForeground(false)
{
	Init();
}

Entity::Entity(const Utils::Vector2<float>& position, const std::string& entityGroup, GridType gridType) :
	position(position),
	animation(AnimationManager::GetNew(AnimationId::NONE)),
	hitbox(HitboxManager::Get(HitboxId::NONE)),
	groups(EntityGroups{ entityGroup }),
	gridType(gridType),
	isRenderedBeforeForeground(false)
{
	Init();
}

void Entity::Init()
{
	isActive = true;
	groups.push_back(Group::ALL);
	destroyEvent = new Event<LPEntity>();
	id = std::to_string(reinterpret_cast<intptr_t>(this));
	isDetectable = true;
}

bool Entity::IsDetectable()
{
	return isActive && isDetectable && !isFreezed;
}

bool Entity::_IsActive()
{
	return isActive;
}

bool Entity::_IsFreezed()
{
	return isFreezed;
}

Entity::~Entity() {
	destroyEvent->Notify(this);
	delete destroyEvent;
	delete animation;
}

LPScene Entity::GetParentScene()
{
	return parentScene;
}

void Entity::SetAnimation(const std::string& id, float animSpeed)
{
	if (id != animation->GetId()) {
		delete animation;
		animation = AnimationManager::GetNew(id);
	}

	animation->SetAnimationSpeed(animSpeed);
}

void Entity::SetHitbox(const std::string& id)
{
	hitbox = HitboxManager::Get(id);
}

const Vector2<float>& Entity::GetPosition()
{
	return position;
}

const Vector2<float>& Entity::GetVelocity()
{
	return velocity;
}

void Entity::SetPosition(const Vector2<float>& position)
{
	this->position = position;
}

void Entity::SetVelocity(const Vector2<float>& velocity)
{
	this->velocity = velocity;
}

void Entity::SetDetectable(bool state)
{
	isDetectable = state;
}

void Entity::Activate(bool state)
{
	isActive = state;
}

void Entity::SetFreeze(bool state)
{
	isFreezed = state;
}

const std::string& Entity::GetId()
{
	return id;
}

GridType Entity::GetGridType() {
	return gridType;
}

const Hitbox& Entity::GetHitbox()
{
	return hitbox;
}

SpriteBox Entity::GetSpriteBox() {
	return animation->GetCurrentSpriteBox();
}

void Entity::OnReady() {
	if (gridType != GridType::STATIC_ENTITIES && gridType != GridType::WALL_ENTITIES)
		parentScene->SubscribeToOutOfWorldEvent(this, &Entity::OnOutOfWorld);
}

void Entity::SetRenderedBeforeForeground(bool state)
{
	isRenderedBeforeForeground = state;
}

void Entity::OnOutOfWorld()
{
	parentScene->QueueFree(this);
}

Utils::Dimension<int> Entity::GetCurrentSpriteDimension()
{
	RECT rect1 = GetSpriteBox().rect;
	return Utils::Dimension<int>(rect1.right - rect1.left, rect1.bottom - rect1.top);
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
	return isRenderedBeforeForeground;
}

const EntityGroups& Entity::GetEntityGroups()
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

