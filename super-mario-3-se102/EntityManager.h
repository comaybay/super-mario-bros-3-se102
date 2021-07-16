#pragma once
#include <unordered_map>
#include <unordered_set>

#include "DynamicGrid.h"
#include "EntityCollection.h"

//Avoid circular dependencies
class Entity;
typedef Entity* LPEntity;

class EntityManager
{
public:
	~EntityManager();
	EntityManager(LPGrid wallEntitySPGrid, LPGrid staticEntitySPGrid, LPDynamicGrid movableEntitySPGrid);
	void Add(LPEntity entity);
	void AddToGroup(std::string groupName, LPEntity entity);
	void AddToGroups(EntityGroups groups, LPEntity entity);
	void ForEach(std::function<void(LPEntity)> handler);
	const EntityCollection& GetEntitiesOfGroup(const std::string& groupName);
	const LPGrid GetGrid(GridType gridType);
	const EntityCollection& GetNonGridEntities();
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
	std::unordered_map<std::string, LPEntityCollection> entitiesByGroup;
	std::unordered_set<LPEntity> freeQueue;
	std::unordered_set<LPEntity> nonGridEntities;
	static const EntityCollection EMPTY_GROUP;

	LPGrid wallEntitySPGrid = nullptr;
	LPGrid staticEntitySPGrid = nullptr;
	LPDynamicGrid movableEntitySPGrid = nullptr;
	bool updateMovablesInSPGrid = true;
	bool renderMovablesInSPGrid = true;
};
typedef EntityManager* LPEntityManager;


