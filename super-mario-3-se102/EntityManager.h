#pragma once
#include <unordered_map>
#include <string>
#include <unordered_set>
#include <list>
#include <functional>

#include "DynamicGrid.h"

//Avoid circular dependencies
class Entity;
typedef Entity* LPEntity;

class EntityManager
{
public:
	~EntityManager();
	EntityManager(LPScene parentScene, LPGrid wallEntitySPGrid, LPGrid staticEntitySPGrid, LPDynamicGrid movableEntitySPGrid);
	void Add(LPEntity entity);
	void AddToGroup(std::string groupName, LPEntity entity);
	void AddToGroups(std::vector<std::string> groups, LPEntity entity);
	void ForEach(std::function<void(LPEntity)> handler);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);
	const LPGrid GetGrid(GridType gridType);
	const std::unordered_set<LPEntity>& GetNonGridEntities();
	void QueueFree(LPEntity entity);
	void FreeEntitiesInQueue();

	/// <summary>
	/// Used internally by SceneLoader
	/// </summary>
	void _AddToNonWallSPGrid(LPEntity entity, const Utils::Vector2<int>& cellIndex);
	/// <summary>
	/// Used internally by SceneLoader
	/// </summary>
	void _AddToWallSPGrid(LPEntity entity, const CellRange& cellRange);
	/// <summary>
	/// Used internally by SceneLoader
	/// </summary>
	void _AddWithoutPutToGrid(LPEntity entity);
private:
	CellRange GetCellRangeAroundCamera();
	LPScene parentScene = nullptr;

	std::unordered_map<std::string, std::list<LPEntity>*> entitiesByGroup;
	std::unordered_set<LPEntity> freeQueue;
	std::unordered_set<LPEntity> nonGridEntities;
	const std::list<LPEntity> emptyGroup;

	LPGrid wallEntitySPGrid = nullptr;
	LPGrid staticEntitySPGrid = nullptr;
	LPDynamicGrid movableEntitySPGrid = nullptr;
	bool updateMovablesInSPGrid = true;
	bool renderMovablesInSPGrid = true;
};
typedef EntityManager* LPEntityManager;


