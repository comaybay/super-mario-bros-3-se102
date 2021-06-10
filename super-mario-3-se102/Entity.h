#pragma once
#include "Utils.h"
#include "Animation.h"

//avoid circular dependency
template<typename ...Args>
class Event;
template<typename ...Args>
using LPEvent = Event<Args...>*;

class Entity;
typedef Entity* LPEntity;

class Entity
{
public:
	Entity(const Utils::Vector2& position, const std::string& initialAnimation, const std::vector<std::string>& entityGroups);
	Entity(const Utils::Vector2& position, const std::string& initialAnimation, const std::string& entityGroup);
	Entity(const Utils::Vector2& position, const std::string& initialAnimation);
	~Entity();
	void SetAnimation(std::string id);
	Utils::Vector2 GetPosition();
	void SetPosition(const Utils::Vector2& position);
	void SetVelocity(const Utils::Vector2& velocity);

	/// <summary>
	/// Get remainging velocity, used for collision detection and collision handling
	/// this is the remaining velocity after any change in position by CollisionHandling.
	/// remaining velocity will be reset every frame.
	/// </summary>
	Utils::Vector2 GetRemainingVelocity();

	void SetRemainingVelocity(Utils::Vector2 vel);
	virtual Utils::Dimension GetDimension();
	Utils::SpriteBox GetSpriteBox();
	virtual void Update(float delta);
	virtual void PostUpdate();
	virtual void Render();
	virtual std::vector<std::string> GetEntityGroups();
	LPEvent<LPEntity> const GetDestroyEvent();
protected:
	std::vector<std::string> groups;
	Utils::Vector2 position;
	Utils::Vector2 velocity;
	LPAnimation animation;
private:
	void Init();
	Utils::Vector2 remainingVelocity;
	LPEvent<LPEntity> destroyEvent; //use LPEvent instead of Event to avoid circular dependency
};
