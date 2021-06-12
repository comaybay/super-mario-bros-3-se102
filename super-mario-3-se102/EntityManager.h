#pragma once
#include <map>
#include <string>
#include <unordered_set>
#include <list>

#include "DynamicGrid.h"

//Avoid circular dependencies
class Entity;
typedef Entity* LPEntity;

class EntityManager
{
public:
	EntityManager(LPScene parentScene, LPGrid wallEntitySpatialGrid, LPGrid staticEntitySpatialGrid, LPDynamicGrid movableEntitySpatialGrid);

	/// <summary>
	/// Add entity at runtime, only used by Scene.
	/// </summary>
	void Add(LPEntity entity);

	void Add(LPEntity entity, const Utils::Vector2<int>& cellIndex);
	void Add(LPEntity entity, const CellRange& cellRange);
	void ForEach(std::function<void(LPEntity)> handler);
	void AddToGroup(std::string groupName, LPEntity entity);
	void AddToGroups(std::vector<std::string> groups, LPEntity entity);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);
	std::vector<LPEntity> GetEntitiesAroundCamera();

	void UpdateAllEntities(float delta);
	void PostUpdateAllEntities();
	void RenderAllEntities();

	void QueueFree(LPEntity entity);
	void FreeEntitiesInQueue();
private:
	CellRange GetCellRangeAroundCamera();

	LPScene parentScene;

	std::map<std::string, std::list<LPEntity>*> entitiesByGroup;
	std::unordered_set<LPEntity> entities;
	std::unordered_set<LPEntity> freeQueue;

	LPGrid wallEntitySpatialGrid;
	LPGrid staticEntitySpatialGrid;
	LPDynamicGrid movableEntitySpatialGrid;
};
typedef EntityManager* LPEntityManager;


