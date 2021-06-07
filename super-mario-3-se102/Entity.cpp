#include "Entity.h"
#include "EntityManager.h"
#include "AnimationManager.h"
#include "Event.h"

Entity::Entity(const Utils::Vector2& position, const std::string& initialAnimation)
	: position(position), animation(AnimationManager::GetNew(initialAnimation)), groups(std::vector<std::string> {})
{
	Init();
}

Entity::Entity(const Utils::Vector2& position, const std::string& initialAnimation, const std::string& entityGroup)
	: position(position), animation(AnimationManager::GetNew(initialAnimation)), groups(std::vector<std::string> {entityGroup})
{
	Init();
}

Entity::Entity(const Utils::Vector2& position, const std::string& initialAnimation, const std::vector<std::string>& entityGroups)
	: position(position), animation(AnimationManager::GetNew(initialAnimation)), groups(entityGroups)
{
	Init();
}

void Entity::Init()
{
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

Utils::Vector2 Entity::GetPosition()
{
	return position;
}

void Entity::SetPosition(const Utils::Vector2& position)
{
	this->position = position;
}

void Entity::SetVelocity(const Utils::Vector2& velocity)
{
	this->velocity = velocity;
}

Utils::Vector2 Entity::GetRemainingVelocity()
{
	return remainingVelocity;
}
void Entity::SetRemainingVelocity(Utils::Vector2 velocity)
{
	remainingVelocity = velocity;
}

Utils::SpriteBox Entity::GetSpriteBox() {
	return animation->GetCurrentSpriteBox();
}

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

std::vector<std::string> Entity::GetEntityGroups()
{
	return groups;
}

