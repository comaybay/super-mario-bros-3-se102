#include "DynamicGrid.h"
#include "Utils.h"
using namespace Utils;

void DynamicGrid::Update()
{
	for (LPEntitiesInCell cell : cells)
		for (LPEntity entity : *cell)
			UpdateEntityCellIndex(entity);
}

void DynamicGrid::Add(LPEntity entity, const Utils::Vector2& cellIndex)
{
	Grid::Add(entity, cellIndex);
	IndexbyLPEntity[entity] = cellIndex;
}

void DynamicGrid::UpdateEntityCellIndex(LPEntity entity)
{
	Utils::Vector2  oldIndex = IndexbyLPEntity[entity];
	Utils::Vector2 newIndex = GetCellIndexAtPoint(entity->GetPosition());

	if (newIndex == oldIndex)
		return;

	//since LPEntity is unique, no need to use list::erase
	cells[oldIndex.y * oldIndex.x + oldIndex.x]->remove(entity);
	cells[newIndex.y * newIndex.x + newIndex.x]->push_back(entity);
}
