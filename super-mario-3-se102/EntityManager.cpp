#include "EntityManager.h"
#include "Groups.h"
#include "Game.h"

std::map<std::string, std::list<LPEntity>*> EntityManager::entitiesByGroup;
std::unordered_set<LPEntity> EntityManager::entities;
std::unordered_set<LPEntity> EntityManager::freeQueue;

void EntityManager::AddToGroup(std::string groupName, LPEntity entity)
{
	if (!Utils::MapHas(groupName, entitiesByGroup))
		entitiesByGroup[groupName] = new std::list<LPEntity>();

	entitiesByGroup[groupName]->push_back(entity);
	entities.insert(entity);
}

void EntityManager::AddToGroups(std::vector<std::string> groups, LPEntity entity)
{
	for (std::string& groupName : groups)
		AddToGroup(groupName, entity);
}

const std::list<LPEntity>& EntityManager::GetGroup(std::string groupName)
{
	if (Utils::MapHas(groupName, entitiesByGroup))
		return *entitiesByGroup[groupName];
	else {
		std::string msg = "No group with name=\"" + groupName + "\" found";
		throw std::exception(msg.c_str());
	}
}

bool EntityManager::IsCloseToPlayer(LPEntity entity)
{
	LPEntity player = entitiesByGroup[Groups::PLAYER]->front();
	float distance = entity->GetPosition().DistanceTo(player->GetPosition());
	return distance <= Game::GetGameDimension().width + 3 * Game::TILE_SIZE;
}

void EntityManager::UpdateAllEntities(float delta)
{
	//update only when close to mario (SMB3 happens to do this)
	//TODO: Make a spawner instead.
	for (auto& entity : entities)
		if (IsCloseToPlayer(entity))
			entity->Update(delta);
}

void EntityManager::PostUpdateAllEntities()
{
	for (auto& groups : entitiesByGroup)
		for (auto& entity : *groups.second)
			entity->PostUpdate();
}

void EntityManager::RenderAllEntities()
{
	for (auto& groups : entitiesByGroup)
		for (auto& entity : *groups.second)
			entity->Render();
}

void EntityManager::QueueFree(LPEntity entity) {
	freeQueue.insert(entity);
}

void EntityManager::FreeEntitiesInQueue() {
	for (LPEntity entity : freeQueue) {
		//remove entity from all of it's groups
		for (std::string& group : entity->GetEntityGroups()) {
			auto removed = std::remove(entitiesByGroup[group]->begin(), entitiesByGroup[group]->end(), entity);
			entitiesByGroup[group]->erase(removed);
		}

		delete entity;
	}

	freeQueue.clear();
}



