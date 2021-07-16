#include "EntityManager.h"
#include "Group.h"
#include "Game.h"
#include "Entity.h"
#include "Scene.h"
#include "Contains.h"
using namespace Utils;

const std::unordered_set<LPEntity> EntityManager::EMPTY_GROUP;

EntityManager::~EntityManager()
{
	std::unordered_set<LPEntity> entities;
	for (auto& pair : entitiesByGroup)
		for (LPEntity e : *pair.second)
			entities.insert(e);

	for (LPEntity e : entities)
		delete e;

	for (auto& pair : entitiesByGroup)
		delete pair.second;

	delete wallEntitySPGrid;
	delete staticEntitySPGrid;
	delete movableEntitySPGrid;
}

EntityManager::EntityManager(LPGrid wallEntitySPGrid, LPGrid staticEntitySPGrid, LPDynamicGrid movableEntitySPGrid)
	: wallEntitySPGrid(wallEntitySPGrid), staticEntitySPGrid(staticEntitySPGrid),
	movableEntitySPGrid(movableEntitySPGrid)
{}

void EntityManager::Add(LPEntity entity)
{
	if (entity->GetGridType() == GridType::NONE) {
		_AddWithoutPutToGrid(entity);
		return;
	}

	if (entity->GetGridType() == GridType::WALL_ENTITIES) {
		CellRange range = wallEntitySPGrid->GetCellRangeFromRectangle(entity->GetPosition(), entity->GetHitbox()->GetDimension());
		_AddToWallSPGrid(entity, range);
		return;
	}

	Vector2<int> cellIndex = movableEntitySPGrid->GetCellIndexAtPoint(entity->GetPosition());
	_AddToNonWallSPGrid(entity, cellIndex);
}


void EntityManager::_AddToNonWallSPGrid(LPEntity entity, const Vector2<int>& cellIndex)
{
	AddToGroups(entity->GetEntityGroups(), entity);

	switch (entity->GetGridType()) {
	case GridType::MOVABLE_ENTITIES:
		movableEntitySPGrid->AddToCell(entity, cellIndex);
		break;
	case GridType::STATIC_ENTITIES:
		staticEntitySPGrid->AddToCell(entity, cellIndex);
		break;
	default:
		throw std::exception("Add failed: for single cellIndex, GridType must either be MOVABLE_ENTITIES or STATIC_ENTITIES");
	}
}

void EntityManager::_AddToWallSPGrid(LPEntity entity, const CellRange& cellRange)
{
	AddToGroups(entity->GetEntityGroups(), entity);

	switch (entity->GetGridType()) {
	case GridType::WALL_ENTITIES:
		for (int y = 0; y < cellRange.span.y; y++)
			for (int x = 0; x < cellRange.span.x; x++)
				wallEntitySPGrid->AddToCell(entity, cellRange.index + Vector2<int>(x, y));
		break;
	default:
		throw std::exception("Add failed: for cellSpan, GridType must be WALL_ENTITIES");
	}
}

void EntityManager::_AddWithoutPutToGrid(LPEntity entity)
{
	AddToGroups(entity->GetEntityGroups(), entity);
	nonGridEntities.insert(entity);
}

void EntityManager::ForEach(std::function<void(LPEntity)> handler) {
	wallEntitySPGrid->ForEachEntity(handler);
	staticEntitySPGrid->ForEachEntity(handler);
	movableEntitySPGrid->ForEachEntity(handler);
	for (LPEntity e : nonGridEntities)
		handler(e);
}

void EntityManager::AddToGroup(std::string groupName, LPEntity entity)
{
	if (!Contains(groupName, entitiesByGroup))
		entitiesByGroup[groupName] = new std::unordered_set<LPEntity>();

	entitiesByGroup[groupName]->insert(entity);
}

void EntityManager::AddToGroups(std::vector<std::string> groups, LPEntity entity)
{
	for (std::string& groupName : groups)
		AddToGroup(groupName, entity);
}

const std::unordered_set<LPEntity>& EntityManager::GetEntitiesOfGroup(const std::string& groupName)
{
	if (Contains(groupName, entitiesByGroup))
		return *entitiesByGroup[groupName];
	else
		return EMPTY_GROUP;
}

const LPGrid EntityManager::GetGrid(GridType gridType)
{
	switch (gridType) {
	case GridType::WALL_ENTITIES:
		return wallEntitySPGrid;
	case GridType::MOVABLE_ENTITIES:
		return movableEntitySPGrid;
	case GridType::STATIC_ENTITIES:
		return staticEntitySPGrid;
	default:
		throw std::exception("GetGrid failed: invalid Gridtype");
	}
}

const std::unordered_set<LPEntity>& EntityManager::GetNonGridEntities()
{
	return nonGridEntities;
}

void EntityManager::QueueFree(LPEntity entity) {
	freeQueue.insert(entity);
}

void EntityManager::FreeEntitiesInQueue() {
	for (LPEntity entity : freeQueue) {
		//remove entity from all of it's groups
		for (const std::string& group : entity->GetEntityGroups())
			entitiesByGroup[group]->erase(entity);

		if (entity->GetGridType() == GridType::NONE)
			nonGridEntities.erase(entity);

		delete entity;
	}

	freeQueue.clear();
}

