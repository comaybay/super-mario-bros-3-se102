#pragma once
#include <string>
#include <vector>
#include "Utils.h"
#include "EncodedWorld.h"
#include "Camera.h"
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
	Utils::Dimension GetWorldDimension();
	Utils::Vector2<float> GetCameraPosition();
	void AddEntity(LPEntity entity);
	void QueueFree(LPEntity entity);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);
	void PlayerDeath();

private:
	void RenderWorld(int(EncodedWorld::* getIndex)(int, int));
	RECT GetTileBoundingBox(int id);
	Utils::Dimension worldTileDim;
	LPEncodedWorld encodedWorld;
	D3DCOLOR backgroundColor;
	Camera camera;
	LPEntityManager entityManager;

public:
	/// <summary>
	/// initialize scene, this method exist because some operations require scene to exist before 
	/// it's constructor's arguments are provided, used internally by SceneManager.
	/// </summary>
	void _Init(Utils::Dimension worldTileDim, D3DCOLOR backgroundColor, LPEncodedWorld encodedWorld, LPEntityManager entityManager);

	/// <summary>
	/// called when everything in the scene is loaded, used internally by SceneManager.
	/// </summary>
	void _Ready();

	/// <summary>
	/// used internally by Game
	/// </summary>
	LPEntityManager _GetEntityManager();
};
typedef Scene* LPScene;

