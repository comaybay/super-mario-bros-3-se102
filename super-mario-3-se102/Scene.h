#pragma once
#include <string>
#include <vector>
#include <unordered_set>

#include "Utils.h"
#include "EncodedWorld.h"
#include "Camera.h"
#include "Grid.h"

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
	const std::string& GetPrevScenePath();
	void AddEntity(LPEntity entity);
	void QueueFree(LPEntity entity);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);
	void PlayerDeath();

	template<class ENTITY>
	using Handler = void(ENTITY::*)();

	template<class ENTITY>
	void SubscribeToOutOfWorldEvent(ENTITY* entity, Handler<ENTITY> handler);

	template<class ENTITY>
	void UnsubscribeToOutOfWorldEvent(ENTITY* entity);

private:
	void OnEntityDestroy(LPEntity entity);
	void NotifyOutOfWorldEntities();
	bool IsOutOfWorld(LPEntity entity);
	std::list<std::pair<LPEntity, Handler<Entity>>> notifyList;
	std::unordered_map<LPEntity, Handler<Entity>> subscriptions;

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
	bool updateMovablesInSPGridEnabled;
	bool renderMovablesInSPGridEnabled;


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
inline void Scene::SubscribeToOutOfWorldEvent(ENTITY* entity, Handler<ENTITY> handler)
{
	subscriptions[entity] = handler;
	entity->GetDestroyEvent().Subscribe(this, &Scene::OnEntityDestroy);
}


template<class ENTITY>
inline void Scene::UnsubscribeToOutOfWorldEvent(ENTITY* entity)
{
	subscriptions.erase(entity);
	entity->GetDestroyEvent().Unsubscribe(this, &Scene::OnEntityDestroy);
}