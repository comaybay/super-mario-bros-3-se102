#pragma once
#include "Hitbox.h"
#include "Animation.h"
#include "GridType.h"

//avoid circular dependencies
template<typename ...ARGS>
class Event;
template<typename ...ARGS>
using LPEvent = Event<ARGS...>*;

class Scene;
typedef Scene* LPScene;

class Entity;
typedef Entity* LPEntity;

typedef std::vector<std::string> EntityGroups;

class Entity
{
public:
	Entity(
		const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
		const EntityGroups& entityGroups, GridType gridType, bool isRenderedBeforeWorld
	);
	Entity(
		const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
		const EntityGroups& entityGroups, GridType gridType
	);
	Entity(
		const Utils::Vector2<float>& position, const std::string& initialAnimation, const std::string& hitbox,
		const std::string& entityGroup, GridType gridType
	);
	Entity(const Utils::Vector2<float>& position, const EntityGroups& entityGroups, GridType gridType);
	Entity(const Utils::Vector2<float>& position, const std::string& entityGroup, GridType gridType);
	virtual ~Entity();
	void SetHitbox(const std::string& id);
	const Utils::Vector2<float>& GetPosition();
	const Utils::Vector2<float>& GetVelocity();
	void SetPosition(const Utils::Vector2<float>& position);
	void SetVelocity(const Utils::Vector2<float>& velocity);
	const std::string& GetId();
	LPScene GetParentScene();
	void SetRenderedBeforeWorld(bool state);
	virtual Utils::Dimension<int> GetCurrentSpriteDimension();
	virtual const Hitbox& GetHitbox();
	SpriteBox GetSpriteBox();
	virtual void Update(float delta);
	virtual void PostUpdate();
	virtual void Render();
	const EntityGroups& GetEntityGroups();
	Event<LPEntity>& GetDestroyEvent();

	/// <summary>
	/// <para> Set entity animation and animation speed. </para>
	/// <para> Given animation speed only affected, When new animation is set, animation speed will be reset. </para>
	/// </summary>
	void SetAnimation(const std::string& id, float animSpeed = 1);

	/// <summary>
	/// If false, entity won't be rendered nor updated and will not include for detection or be detected by CollisionEngine,
	/// but is not removed from the scene
	/// </summary>
	void Activate(bool state);

	/// <summary>
	/// If true, entity won't be updated but will be rendered,
	/// </summary>
	void SetFreeze(bool state);

	/// <summary>
	/// Set if is detectable by CollisionEngine
	/// </summary>
	void SetDetectable(bool state);

	/// <summary>
	/// returns true if entity is able to be detected by CollisionEngine.
	/// </summary>
	/// <returns></returns>
	bool IsDetectable();

	/// <summary>
	/// GridType will be used for spatial parititoning
	/// </summary>
	GridType GetGridType();

	/// <summary>
	/// Called when entity's parent scene is ready and are now allowed to subscribe to CollisionEngine.
	/// Override this method when your entity needs to do operations that require parent scene.
	/// </summary>
	virtual void OnReady();

	/// <summary>
	/// Used internaly by Scene to inject entity's parent scene
	/// </summary>
	void _SetParentScene(LPScene scene);

	/// <summary>
	/// Used internaly by Scene to see if entity want to render before or after world.
	/// </summary>
	/// <returns></returns>
	bool _IsRenderedBeforeWorld();

	/// <summary>
	/// Used internaly by CollisionEngine and Scene to see if entity is active.
	/// </summary>
	/// <returns></returns>
	bool _IsActive();

	/// <summary>
	/// Used internaly by Scene to see if entity is freezed.
	/// </summary>
	/// <returns></returns>
	bool _IsFreezed();

	/// <summary>
	/// remainging velocity is used internaly by CollisionEngine and CollisionHandling
	/// this is the remaining velocity after any change in position by CollisionHandling.
	/// remaining velocity will be reset every frame.
	/// </summary>
	Utils::Vector2<float> _GetRemainingVelocity();
	void _SetRemainingVelocity(Utils::Vector2<float> vel);

protected:
	virtual void OnOutOfWorld();
	LPScene parentScene;
	EntityGroups groups;
	Utils::Vector2<float> position;
	Utils::Vector2<float> velocity;
	LPAnimation animation;
	Hitbox hitbox;
private:
	void Init();
	std::string id;
	bool isRenderedBeforeWorld;
	bool isDetectable;
	bool isActive;
	bool isFreezed;
	GridType gridType;
	Utils::Vector2<float> remainingVelocity;
	LPEvent<LPEntity> destroyEvent; //use LPEvent instead of Event to avoid circular dependency
};

