#pragma once
#include <string>

#include "EncodedWorld.h"
#include "Camera.h"
#include "HUD.h"
#include "Timer.h"
#include "Grid.h"
#include "Event.h"
#include "EntityCollection.h"

//avoid circular dependecies
class Entity;
typedef Entity* LPEntity;

class EntityManager;
typedef EntityManager* LPEntityManager;

class Scene
{
public:
	~Scene();
	Scene();
	virtual void Update(float delta);
	virtual void Render();
	const std::string& GetScenePath();
	Utils::Dimension<int> GetWorldDimension();
	const Utils::Vector2<float>& GetCameraPosition();
	Camera& GetCamera();
	void StopTimer();
	const std::string& GetPrevScenePath();
	bool IsEntityGroupEmpty(const std::string& groupName);

	const EntityCollection& GetEntitiesOfGroup(const std::string& groupName);
	void ForEachNonWallEntityOnCamera(const std::function<void(LPEntity)>& handler);

	/// <summary>
	/// Add given entity after current frame.
	/// </summary>
	void AddEntity(LPEntity entity);

	/// <summary>
	/// <para> Add given entity after current frame. 
	/// Given callback will be called when entity's OnReady method is called. </para>
	/// <para> Use this when you want manipulate entity when it is ready. </para>
	/// </summary>
	void AddEntity(LPEntity entity, const std::function<void(LPEntity)>& callback);

	/// <summary>
	/// Free given entity after current frame.
	/// </summary>
	void QueueFree(LPEntity entity);

	/// <summary>
	/// Returns an entity of given group. This entity is not guarantee to be the first element in the group.
	/// </summary>
	const LPEntity GetEntityOfGroup(const std::string& groupName);

	/// <summary>
	/// Pause used for transitions (player's death, player's power level changed)
	/// </summary>
	void TransitionPause(bool state);

	template<class ENTITY>
	void SubscribeToOutOfWorldEvent(ENTITY* entity, void(ENTITY::* handler)());

	template<class ENTITY>
	void UnsubscribeToOutOfWorldEvent(ENTITY* entity);

protected:
	void DetectAndNotifyOutOfWorld();
	void OnEntityDestroy(LPEntity entity);
	bool IsOutOfWorld(LPEntity entity);

	CellRange GetCellRangeAroundCamera();
	RECT GetTileBoundingBox(int id);
	void RenderWorld(int(EncodedWorld::* getIndex)(int, int));
	void GetRenderEntities(std::vector<LPEntity>& entitiesRenderedBeforeWorld, std::vector<LPEntity>& entitiesRenderedrAfterWorld);
	std::string scenePath;
	Utils::Dimension<int> worldTileDim;
	LPEncodedWorld encodedWorld;
	D3DCOLOR backgroundColor;
	std::string prevScenePath;
	HUD hud;
	Timer timer;
	Camera camera;
	LPEntityManager entityManager;
	bool isInTransitionPause;
	bool renderMovablesInSPGridEnabled;
	std::unordered_set<LPEntity> newEntitiesWaitList;
	std::unordered_map<LPEntity, std::function<void(LPEntity)>> afterOnReadyCallbackByLPEntity;
	std::unordered_map<LPEntity, LPEvent<>> outOfWorldEventByLPEntity;
	std::unordered_set<LPEntity> outOutWorldUnsubscribeWaitList;

public:
	/// <summary>
	/// initialize scene, this method exist because some operations require scene to exist before 
	/// it's constructor's arguments are provided, used internally by SceneLoader.
	/// </summary>
	void _Init(const std::string& scenePath, const Utils::Dimension<int>& worldTileDim, const D3DCOLOR& backgroundColor, LPEncodedWorld encodedWorld, LPEntityManager entityManager, const std::string& prevScenePath);

	/// <summary>
	/// called when everything in the scene is loaded, used internally by SceneLoader.
	/// </summary>
	virtual void _Ready();

	/// <summary>
	/// Free entities, used internally by Game
	/// </summary>
	void _FreeEntitiesInQueue();

};
typedef Scene* LPScene;


template<class ENTITY>
inline void Scene::SubscribeToOutOfWorldEvent(ENTITY* entity, void(ENTITY::* handler)())
{
	if (!Utils::Contains(entity, outOfWorldEventByLPEntity))
		outOfWorldEventByLPEntity[entity] = new Event<>();

	outOfWorldEventByLPEntity[entity]->Subscribe(entity, handler);
	entity->GetDestroyEvent().Subscribe(this, &Scene::OnEntityDestroy);
}


template<class ENTITY>
inline void Scene::UnsubscribeToOutOfWorldEvent(ENTITY* entity)
{
	outOutWorldUnsubscribeWaitList.insert(entity);
	entity->GetDestroyEvent().Unsubscribe(this, &Scene::OnEntityDestroy);
}