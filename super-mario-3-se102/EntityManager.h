#pragma once
#include <map>
#include <string>
#include <list>
#include <unordered_set>
#include "Entity.h"

typedef Entity* LPEntity;

class EntityManager
{
public:
	static void AddToGroup(std::string groupName, LPEntity entity);
	static void AddToGroups(std::vector<std::string> groups, LPEntity entity);
	static void RemoveFromGroup(std::string groupName, LPEntity entity);
	static const std::list<LPEntity>& GetGroup(std::string groupName);
	static void UpdateAllEntities(float delta);
	static void PostUpdateAllEntities();
	static void RenderAllEntities();
	static void QueueFree(LPEntity entity);
	static void FreeEntitiesInQueue();
private:
	static bool IsCloseToPlayer(LPEntity entity);
	static std::map<std::string, std::list<LPEntity>*> entitiesByGroup;
	static std::unordered_set<LPEntity> entities;
	static std::unordered_set<LPEntity> freeQueue;
};

