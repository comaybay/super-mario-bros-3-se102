#include "EntityManager.h"
#include "Groups.h"
#include "Game.h"
#include "Entity.h"
#include "Scene.h"
using namespace Utils;

EntityManager::EntityManager
(LPScene parentScene, LPGrid wallEntitySpatialGrid, LPGrid staticEntitySpatialGrid, LPDynamicGrid movableEntitySpatialGrid)
	: parentScene(parentScene), wallEntitySpatialGrid(wallEntitySpatialGrid), staticEntitySpatialGrid(staticEntitySpatialGrid),
	movableEntitySpatialGrid(movableEntitySpatialGrid)
{}

void EntityManager::Add(LPEntity entity)
{
	if (entity->GetGridType() == GridType::WALL_ENTITIES) {
		CellRange range = wallEntitySpatialGrid->GetCellRangeFromRectangle(entity->GetPosition(), entity->GetDimension());
		Add(entity, range);
		return;
	}

	Vector2<int> cellIndex = movableEntitySpatialGrid->GetCellIndexAtPoint(entity->GetPosition());
	Add(entity, cellIndex);
}

void EntityManager::Add(LPEntity entity, const Vector2<int>& cellIndex)
{
	AddToGroups(entity->GetEntityGroups(), entity);
	entity->_SetParentScene(parentScene);

	switch (entity->GetGridType()) {
	case GridType::MOVABLE_ENTITIES:
		movableEntitySpatialGrid->AddToCell(entity, cellIndex);
		break;
	case GridType::STATIC_ENTITIES:
		staticEntitySpatialGrid->AddToCell(entity, cellIndex);
		break;
	default:
		throw std::exception("Add failed: for single cellIndex, GridType must either be MOVABLE_ENTITIES or STATIC_ENTITIES");
	}
}

void EntityManager::Add(LPEntity entity, const CellRange& cellRange)
{
	AddToGroups(entity->GetEntityGroups(), entity);
	entity->_SetParentScene(parentScene);

	switch (entity->GetGridType()) {
	case GridType::WALL_ENTITIES:
		for (int y = 0; y < cellRange.span.y; y++)
			for (int x = 0; x < cellRange.span.x; x++)
				wallEntitySpatialGrid->AddToCell(entity, cellRange.index + Vector2<int>(x, y));
		break;
	default:
		throw std::exception("Add failed: for cellSpan, GridType must be WALL_ENTITIES");
	}
}

void EntityManager::ForEach(std::function<void(LPEntity)> handler) {
	wallEntitySpatialGrid->ForEachEntity(handler);
	staticEntitySpatialGrid->ForEachEntity(handler);
	movableEntitySpatialGrid->ForEachEntity(handler);
}

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

const std::list<LPEntity>& EntityManager::GetEntitiesByGroup(std::string groupName)
{
	if (Utils::MapHas(groupName, entitiesByGroup))
		return *entitiesByGroup[groupName];
	else {
		std::string msg = "No group with name=\"" + groupName + "\" found";
	}
}

std::vector<LPEntity> EntityManager::GetEntitiesAroundCamera()
{
	CellRange range = GetCellRangeAroundCamera();

	std::vector<LPEntity> entities;

	for (int y = 0; y < range.span.y; y++)
		for (int x = 0; x < range.span.x; x++) {
			LPConstEntitiesInCell walls = wallEntitySpatialGrid->EntitiesAt(range.index + Vector2<int>(x, y));
			LPConstEntitiesInCell statics = staticEntitySpatialGrid->EntitiesAt(range.index + Vector2<int>(x, y));
			LPConstEntitiesInCell movables = movableEntitySpatialGrid->EntitiesAt(range.index + Vector2<int>(x, y));

			entities.insert(entities.end(), walls->begin(), walls->end());
			entities.insert(entities.end(), statics->begin(), statics->end());
			entities.insert(entities.end(), movables->begin(), movables->end());
		}

	return entities;
}

void EntityManager::UpdateAllEntities(float delta)
{
	CellRange range = GetCellRangeAroundCamera();
	auto updateHandler = [delta](LPEntity entity) { entity->Update(delta); };
	staticEntitySpatialGrid->ForEachEntityIn(range, updateHandler);
	movableEntitySpatialGrid->ForEachEntityIn(range, updateHandler);
	movableEntitySpatialGrid->UpdateCells(range);
}

void EntityManager::PostUpdateAllEntities()
{
	CellRange range = GetCellRangeAroundCamera();
	auto postUpdateHandler = [](LPEntity entity) { entity->PostUpdate(); };
	staticEntitySpatialGrid->ForEachEntityIn(range, postUpdateHandler);
	movableEntitySpatialGrid->ForEachEntityIn(range, postUpdateHandler);
}

void EntityManager::RenderAllEntities()
{
	CellRange range = GetCellRangeAroundCamera();
	auto renderHandler = [](LPEntity entity) { entity->Render(); };
	movableEntitySpatialGrid->ForEachEntityIn(range, renderHandler);
	staticEntitySpatialGrid->ForEachEntityIn(range, renderHandler);
}

CellRange EntityManager::GetCellRangeAroundCamera() {
	Vector2<float> camPos = parentScene->GetCameraPosition();
	Dimension dim = Game::GetGameDimension();
	return staticEntitySpatialGrid->GetCellRangeFromRectangle(camPos, dim);
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

