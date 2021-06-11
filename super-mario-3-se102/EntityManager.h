#pragma once
#include <map>
#include <string>
#include <unordered_set>
#include <list>

#include "Entity.h"
#include "DynamicGrid.h"

class EntityManager
{
public:
	EntityManager(LPGrid wallEntitySpatialGrid, LPGrid staticEntitySpatialGrid, LPDynamicGrid movableEntitySpatialGrid);
	void Add(LPEntity entity);
	void Add(LPEntity entity, const Utils::Vector2<int>& cellIndex);
	void AddToGroup(std::string groupName, LPEntity entity);
	void AddToGroups(std::vector<std::string> groups, LPEntity entity);
	void RemoveFromGroup(std::string groupName, LPEntity entity);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);
	std::vector<LPEntity> GetEntitiesAroundCamera();
	void UpdateAllEntities(float delta);
	void PostUpdateAllEntities();
	void RenderAllEntities();
	void QueueFree(LPEntity entity);
	void FreeEntitiesInQueue();
private:
	bool IsCloseToPlayer(LPEntity entity);
	std::map<std::string, std::list<LPEntity>*> entitiesByGroup;
	std::unordered_set<LPEntity> entities;
	std::unordered_set<LPEntity> freeQueue;

	LPGrid wallEntitySpatialGrid;
	LPGrid staticEntitySpatialGrid;
	LPDynamicGrid movableEntitySpatialGrid;
};
typedef EntityManager* LPEntityManager;
