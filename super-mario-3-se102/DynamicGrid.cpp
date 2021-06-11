#include "DynamicGrid.h"
#include "Utils.h"
using namespace Utils;

DynamicGrid::DynamicGrid(int numberOfColumns, int numberOfRows, const Utils::Dimension& cellSize)
	: Grid(numberOfColumns, numberOfRows, cellSize)
{}

void DynamicGrid::Update()
{
	for (LPEntitiesInCell cell : cells)
		for (LPEntity entity : *cell)
			UpdateEntityCellIndex(entity);
}

void DynamicGrid::AddToCell(LPEntity entity, const Utils::Vector2<int>& cellIndex)
{
	Grid::AddToCell(entity, cellIndex);
	IndexbyLPEntity[&(*entity)] = cellIndex;
}

void DynamicGrid::UpdateEntityCellIndex(LPEntity entity)
{
	Utils::Vector2<int> oldIndex = IndexbyLPEntity[entity];
	Utils::Vector2<int> newIndex = GetCellIndexAtPoint(entity->GetPosition());

	if (newIndex == oldIndex)
		return;

	//since LPEntity is unique, no need to use list::erase
	cells[int(oldIndex.y * oldIndex.x + oldIndex.x)]->remove(entity);
	cells[int(newIndex.y * newIndex.x + newIndex.x)]->push_back(entity);
}
