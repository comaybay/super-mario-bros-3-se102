#pragma once
#include "Hitbox.h"
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
	Entity(
		const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
		const std::vector<std::string>& entityGroups, GridType gridType, bool isRenderedBeforeWorld
	);
	Entity(
		const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
		const std::vector<std::string>& entityGroups, GridType gridType
	);
	Entity(
		const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
		const std::string& entityGroup, GridType gridType
	);
	Entity(const Utils::Vector2<float>& position, const std::vector<std::string>& entityGroups, GridType gridType);
	Entity(const Utils::Vector2<float>& position, const std::string& entityGroup, GridType gridType);
	virtual ~Entity();
	/// <summary>
	/// <para> Set entity animation and animation speed. </para>
	/// <para> Given animation speed only affected, When new animation is set, animation speed will be reset. </para>
	/// </summary>
	void SetAnimation(const std::string& id, float animSpeed = 1);
	void SetHitbox(const std::string& id);
	const Utils::Vector2<float>& GetPosition();
	const Utils::Vector2<float>& GetVelocity();
	void SetPosition(const Utils::Vector2<float>& position);
	void SetVelocity(const Utils::Vector2<float>& velocity);
	void EnableForCollisionDetection(bool state);
	const std::string& GetId();
	LPScene GetParentScene();
	void UnsubscribeToOutOfWorldEvent();
	void SubscribeToOutOfWorldEvent();
	void SetRenderedBeforeWorld(bool state);

	/// <summary>
	/// If false, entity won't be rendered nor updated, not affected by collision engine, but is not removed from the scene
	/// </summary>
	/// <param name="state"></param>
	void Activate(bool state);

	/// <summary>
	/// GridType will be used for spatial parititoning
	/// </summary>
	GridType GetGridType();

	virtual Utils::Dimension<int> GetCurrentSpriteDimension();
	virtual LPConstHitbox GetHitbox();
	SpriteBox GetSpriteBox();
	virtual void Update(float delta);
	virtual void PostUpdate();
	virtual void Render();
	const std::vector<std::string>& GetEntityGroups();
	Event<LPEntity>& GetDestroyEvent();

	/// <summary>
	/// Called when it's parent scene ready, override this when entity need to do operations that require parent scene or EntityManager
	/// </summary>
	virtual void OnReady();

	/// <summary>
	/// Used internaly by SceneLoader to inject entity's parent scene
	/// </summary>
	void _SetParentScene(LPScene scene);

	/// <summary>
	/// Used internaly by Scene to see if entity want to render before or after world.
	/// </summary>
	/// <returns></returns>
	bool _IsRenderedBeforeWorld();

	/// <summary>
	/// Used internaly by CollisionEngine to see if entity want to detect collision.
	/// </summary>
	/// <returns></returns>
	bool _IsEnabledForCollisionDetection();

	/// <summary>
	/// remainging velocity is used internaly by CollisionEngine and CollisionHandling
	/// this is the remaining velocity after any change in position by CollisionHandling.
	/// remaining velocity will be reset every frame.
	/// </summary>
	Utils::Vector2<float> _GetRemainingVelocity();
	void _SetRemainingVelocity(Utils::Vector2<float> vel);
protected:
	LPScene parentScene;
	std::vector<std::string> groups;
	Utils::Vector2<float> position;
	Utils::Vector2<float> velocity;
	LPAnimation animation;
	LPConstHitbox hitbox;
	virtual void OnOutOfWorld();
private:
	void Init();
	std::string id;
	bool isRenderedBeforeWorld;
	bool enabledForCollisionDetection;
	bool isActive;
	GridType gridType;
	Utils::Vector2<float> remainingVelocity;
	LPEvent<LPEntity> destroyEvent; //use LPEvent instead of Event to avoid circular dependency
};
