#include "EntityManager.h"
#include "Group.h"
#include "Game.h"
#include "Entity.h"
#include "Scene.h"
#include "Contains.h"
using namespace Utils;

EntityManager::EntityManager
(LPScene parentScene, LPGrid wallEntitySPGrid, LPGrid staticEntitySPGrid, LPDynamicGrid movableEntitySPGrid)
	: parentScene(parentScene), wallEntitySPGrid(wallEntitySPGrid), staticEntitySPGrid(staticEntitySPGrid),
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
	entity->_SetParentScene(parentScene);

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
	entity->_SetParentScene(parentScene);

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
	entity->_SetParentScene(parentScene);
	nonGridEntities.insert(entity);
}

void EntityManager::ForEach(std::function<void(LPEntity)> handler) {
	wallEntitySPGrid->ForEachEntity(handler);
	staticEntitySPGrid->ForEachEntity(handler);
	movableEntitySPGrid->ForEachEntity(handler);
}

void EntityManager::AddToGroup(std::string groupName, LPEntity entity)
{
	if (!Contains(groupName, entitiesByGroup))
		entitiesByGroup[groupName] = new std::list<LPEntity>();

	entitiesByGroup[groupName]->push_back(entity);
}

void EntityManager::AddToGroups(std::vector<std::string> groups, LPEntity entity)
{
	for (std::string& groupName : groups)
		AddToGroup(groupName, entity);
}

const std::list<LPEntity>& EntityManager::GetEntitiesByGroup(std::string groupName)
{
	if (Contains(groupName, entitiesByGroup))
		return *entitiesByGroup[groupName];
	else
		return emptyGroup;
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

CellRange EntityManager::GetCellRangeAroundCamera() {
	Vector2<float> camPos = parentScene->GetCameraPosition();
	Dimension dim = Game::GetGameDimension();

	//add margin
	int marginSize = 16 * 1;
	camPos = camPos - Vector2<float>(marginSize, marginSize);
	dim.width += marginSize;
	dim.height += marginSize;

	return staticEntitySPGrid->GetCellRangeFromRectangle(camPos, dim);
}

void EntityManager::QueueFree(LPEntity entity) {
	freeQueue.insert(entity);
}

void EntityManager::FreeEntitiesInQueue() {
	for (LPEntity entity : freeQueue) {
		//remove entity from all of it's groups
		for (const std::string& group : entity->GetEntityGroups()) {
			auto removed = std::remove(entitiesByGroup[group]->begin(), entitiesByGroup[group]->end(), entity);
			entitiesByGroup[group]->erase(removed);
		}

		if (entity->GetGridType() == GridType::NONE)
			nonGridEntities.erase(entity);

		delete entity;
	}

	freeQueue.clear();
}

