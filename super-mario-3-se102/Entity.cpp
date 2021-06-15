#include "Entity.h"
#include "EntityManager.h"
#include "AnimationManager.h"
#include "Event.h"
#include "Scene.h"
#include "Groups.h"

using namespace Utils;

Entity::Entity(const Utils::Vector2<float>& position, const std::string& initialAnimation, GridType gridType)
	: position(position), animation(AnimationManager::GetNew(initialAnimation)),
	groups(std::vector<std::string> {}), gridType(gridType)
{
	Init();
}

Entity::Entity(const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& entityGroup, GridType gridType)
	: position(position), animation(AnimationManager::GetNew(initialAnimation)),
	groups(std::vector<std::string> {entityGroup}), gridType(gridType)
{
	Init();
}

Entity::Entity
(const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::vector<std::string>& entityGroups, GridType gridType)
	: position(position), animation(AnimationManager::GetNew(initialAnimation)),
	groups(entityGroups), gridType(gridType)
{
	Init();
}

void Entity::Init()
{
	groups.push_back(Groups::ALL);
	destroyEvent = new Event<LPEntity>();
}

Entity::~Entity() {
	destroyEvent->Notify(this);
	delete destroyEvent;
}

void Entity::SetAnimation(std::string id)
{
	if (id != animation->id) {
		delete animation;
		animation = AnimationManager::GetNew(id);
	}
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

GridType Entity::GetGridType() {
	return gridType;
}

Utils::SpriteBox Entity::GetSpriteBox() {
	return animation->GetCurrentSpriteBox();
}

//do nothing by default
void Entity::OnReady() {}

Utils::Dimension Entity::GetDimension()
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

LPEvent<LPEntity> const Entity::GetDestroyEvent()
{
	return destroyEvent;
}

LPScene Entity::GetParentScene()
{
	return parentScene;
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
