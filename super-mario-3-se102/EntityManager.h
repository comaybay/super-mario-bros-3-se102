#pragma once
#include <map>
#include <string>
#include <list>
#include <unordered_set>
#include "Entity.h"

class EntityManager
{
public:
	void AddToGroup(std::string groupName, LPEntity entity);
	void AddToGroups(std::vector<std::string> groups, LPEntity entity);
	void RemoveFromGroup(std::string groupName, LPEntity entity);
	const std::list<LPEntity>& GetEntitiesByGroup(std::string groupName);
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
};
typedef EntityManager* LPEntityManager;
