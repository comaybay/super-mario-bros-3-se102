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
	EntityManager(LPScene parentScene, LPGrid wallEntitySPGrid, LPGrid staticEntitySPGrid, LPDynamicGrid movableEntitySPGrid);

	/// <summary>
	/// Add entity at runtime, only used by Scene.
	/// </summary>
	void Add(LPEntity entity);

	/// <summary>
	/// Used internally by SceneManager
	/// </summary>
	void _AddToNonWallSPGrid(LPEntity entity, const Utils::Vector2<int>& cellIndex);
	/// <summary>
	/// Used internally by SceneManager
	/// </summary>
	void _AddToWallSPGrid(LPEntity entity, const CellRange& cellRange);
	/// <summary>
	/// Used internally by SceneManager
	/// </summary>
	void _AddWithoutPutToGrid(LPEntity entity);

	void ForEach(std::function<void(LPEntity)> handler);
	void AddToGroup(std::string groupName, LPEntity entity);
	void AddToGroups(std::vector<std::string> groups, LPEntity entity);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);

	void UpdateEntities(float delta);
	void PostUpdateEntities();
	void RenderEntities();
	void SetUpdateEntitiesInMovableSPGrid(bool state);
	void SetRenderEntitiesInMovableSPGrid(bool state);

	void QueueFree(LPEntity entity);
	void FreeEntitiesInQueue();
private:
	CellRange GetCellRangeAroundCamera();
	LPScene parentScene = nullptr;

	std::map<std::string, std::list<LPEntity>*> entitiesByGroup;
	std::unordered_set<LPEntity> freeQueue;
	std::list<LPEntity> nonGridEntities;

	LPGrid wallEntitySPGrid = nullptr;
	LPGrid staticEntitySPGrid = nullptr;
	LPDynamicGrid movableEntitySPGrid = nullptr;
	bool updateMovablesInSPGrid = true;
	bool renderMovablesInSPGrid = true;
};
typedef EntityManager* LPEntityManager;


