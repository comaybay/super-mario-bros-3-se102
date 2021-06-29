#pragma once
#include <string>
#include <vector>
#include "Utils.h"
#include "EncodedWorld.h"
#include "Camera.h"
#include "Event.h"
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
	Utils::Dimension GetWorldDimension();
	Utils::Vector2<float> GetCameraPosition();
	void AddEntity(LPEntity entity);
	void QueueFree(LPEntity entity);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);
	void PlayerDeath();

private:
	CellRange GetCellRangeAroundCamera();
	RECT GetTileBoundingBox(int id);
	void RenderWorld(int(EncodedWorld::* getIndex)(int, int));
	void RenderEntitiesInGrids();
	Utils::Dimension worldTileDim;
	LPEncodedWorld encodedWorld;
	D3DCOLOR backgroundColor;
	Camera camera;
	LPEntityManager entityManager;
	bool updateMovablesInSPGridEnabled;
	bool renderMovablesInSPGridEnabled;


public:
	/// <summary>
	/// initialize scene, this method exist because some operations require scene to exist before 
	/// it's constructor's arguments are provided, used internally by SceneLoader.
	/// </summary>
	void _Init(Utils::Dimension worldTileDim, D3DCOLOR backgroundColor, LPEncodedWorld encodedWorld, LPEntityManager entityManager);

	/// <summary>
	/// called when everything in the scene is loaded, used internally by SceneLoader.
	/// </summary>
	void _Ready();
};
typedef Scene* LPScene;

