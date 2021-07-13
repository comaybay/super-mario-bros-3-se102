#pragma once
#include <string>
#include <vector>
#include <unordered_set>

#include "Utils.h"
#include "EncodedWorld.h"
#include "Camera.h"
#include "Grid.h"
#include "Event.h"

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
	void Update(float delta);
	void Render();
	Utils::Dimension<int> GetWorldDimension();
	const Utils::Vector2<float>& GetCameraPosition();
	Camera& GetCamera();
	const std::string& GetPrevScenePath();
	void AddEntity(LPEntity entity);
	void QueueFree(LPEntity entity);
	bool IsEntityGroupEmpty(const std::string& groupName);
	const std::unordered_set<LPEntity>& GetEntitiesOfGroup(const std::string& groupName);

	/// <summary>
	/// Returns an entity of given group. This entity is not guarantee to be the first element in the group.
	/// </summary>
	const LPEntity GetEntityOfGroup(const std::string& groupName);

	/// <summary>
	/// Pause used for transitions (player's death, player's power level changed)
	/// </summary>
	void TransitionPause(bool state);

	template<class ENTITY>
	void SubscribeToOutOfWorldEvent(ENTITY* entity, Event<>::MethodHandler<ENTITY> handler);

	template<class ENTITY>
	void UnsubscribeToOutOfWorldEvent(ENTITY* entity);

private:

	void DetectAndNotifyOutOfWorld();
	void OnEntityDestroy(LPEntity entity);
	bool IsOutOfWorld(LPEntity entity);

	CellRange GetCellRangeAroundCamera();
	RECT GetTileBoundingBox(int id);
	void RenderWorld(int(EncodedWorld::* getIndex)(int, int));
	void GetRenderEntities(std::list<LPEntity>& entitiesRenderedBeforeWorld, std::list<LPEntity>& entitiesRenderedrAfterWorld);
	Utils::Dimension<int> worldTileDim;
	LPEncodedWorld encodedWorld;
	D3DCOLOR backgroundColor;
	std::string prevScenePath;
	Camera camera;
	LPEntityManager entityManager;
	bool isInTransitionPause;
	bool renderMovablesInSPGridEnabled;
	std::unordered_set<LPEntity> newEntitiesWaitList;
	std::unordered_map<LPEntity, LPEvent<>> outOfWorldEventByLPEntity;

public:
	/// <summary>
	/// initialize scene, this method exist because some operations require scene to exist before 
	/// it's constructor's arguments are provided, used internally by SceneLoader.
	/// </summary>
	void _Init(const Utils::Dimension<int>& worldTileDim, const D3DCOLOR& backgroundColor, LPEncodedWorld encodedWorld, LPEntityManager entityManager, const std::string& prevScenePath);

	/// <summary>
	/// called when everything in the scene is loaded, used internally by SceneLoader.
	/// </summary>
	void _Ready();
};
typedef Scene* LPScene;


template<class ENTITY>
inline void Scene::SubscribeToOutOfWorldEvent(ENTITY* entity, Event<>::MethodHandler<ENTITY> handler)
{
	if (!Utils::Contains(entity, outOfWorldEventByLPEntity))
		outOfWorldEventByLPEntity[entity] = new Event<>();

	outOfWorldEventByLPEntity[entity]->Subscribe(entity, handler);
	entity->GetDestroyEvent().Subscribe(this, &Scene::OnEntityDestroy);
}


template<class ENTITY>
inline void Scene::UnsubscribeToOutOfWorldEvent(ENTITY* entity)
{
	outOfWorldEventByLPEntity[entity]->Unsubscribe(entity);
	entity->GetDestroyEvent().Unsubscribe(this, &Scene::OnEntityDestroy);
}