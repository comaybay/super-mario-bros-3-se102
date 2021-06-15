#pragma once
#include "Utils.h"
#include "Animation.h"
#include "GridType.h"

//avoid circular dependencies
template<typename ...Args>
class Event;
template<typename ...Args>
using LPEvent = Event<Args...>*;

class Scene;
typedef Scene* LPScene;

class Entity;
typedef Entity* LPEntity;

class Entity
{
public:
	Entity(const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::vector<std::string>& entityGroups, GridType gridType);
	Entity(const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& entityGroup, GridType gridType);
	Entity(const Utils::Vector2<float>& position, const std::string& initialAnimation, GridType gridType);
	~Entity();
	void SetAnimation(std::string id);
	Utils::Vector2<float> GetPosition();
	void SetPosition(const Utils::Vector2<float>& position);
	void SetVelocity(const Utils::Vector2<float>& velocity);
	LPScene GetParentScene();

	/// <summary>
	/// Called when it's parent scene ready, override this when entity need to do operations that require parent scene or EntityManager
	/// </summary>
	virtual void OnReady();

	/// <summary>
	/// Used internaly SceneManager to inject entity's parent scene
	/// </summary>
	void _SetParentScene(LPScene scene);

	/// <summary>
	/// remainging velocity is used internaly by CollisionEngine and CollisionHandling
	/// this is the remaining velocity after any change in position by CollisionHandling.
	/// remaining velocity will be reset every frame.
	/// </summary>
	Utils::Vector2<float> _GetRemainingVelocity();
	void _SetRemainingVelocity(Utils::Vector2<float> vel);

	/// <summary>
	/// GridType will be used for spatial parititoning
	/// </summary>
	GridType GetGridType();

	virtual Utils::Dimension GetDimension();
	Utils::SpriteBox GetSpriteBox();
	virtual void Update(float delta);
	virtual void PostUpdate();
	virtual void Render();
	virtual std::vector<std::string> GetEntityGroups();
	LPEvent<LPEntity> const GetDestroyEvent();
protected:
	std::vector<std::string> groups;
	Utils::Vector2<float> position;
	Utils::Vector2<float> velocity;
	LPAnimation animation;
private:
	LPScene parentScene;
	void Init();
	GridType gridType;
	Utils::Vector2<float> remainingVelocity;
	LPEvent<LPEntity> destroyEvent; //use LPEvent instead of Event to avoid circular dependency
};
