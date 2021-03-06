#include "DynamicGrid.h"
#include "Event.h"
#include "Utils.h"
using namespace Utils;

DynamicGrid::DynamicGrid(int numberOfColumns, int numberOfRows, const Utils::Dimension<float>& cellSize)
	: Grid(numberOfColumns, numberOfRows, cellSize)
{}

void DynamicGrid::UpdateCells(const CellRange& range)
{
	for (int y = 0; y < range.span.y; y++)
		for (int x = 0; x < range.span.x; x++) {
			const EntityCollection& entities = EntitiesAt(range.index + Vector2<int>(x, y));
			//this process require removing entity in the cell while iterating, therefore an iterator must be used 
			for (auto it = entities.begin(); it != entities.end();) {
				size_t prevSize = entities.size();
				auto newIt = UpdateEntityCellIndex(it);

				if (prevSize == entities.size())
					it++;
				else
					it = newIt;
			}
		};
}

void DynamicGrid::AddToCell(LPEntity entity, const Utils::Vector2<int>& cellIndex)
{
	Grid::AddToCell(entity, cellIndex);
	entity->GetDestroyEvent().Subscribe(this, &DynamicGrid::OnEntityDestroy);
	indexbyLPEntity[entity] = cellIndex;
}

EntityCollection::const_iterator DynamicGrid::UpdateEntityCellIndex(EntityCollection::const_iterator it)
{
	LPEntity entity = *it;
	Utils::Vector2<int> oldIndex = indexbyLPEntity[entity];
	Utils::Vector2<int> newIndex = GetCellIndexAtPoint(entity->GetPosition());

	if (newIndex == oldIndex)
		return it;

	auto newIt = cells[int(oldIndex.y * numOfCols + oldIndex.x)]->erase(it);
	cells[int(newIndex.y * numOfCols + newIndex.x)]->insert(entity);
	indexbyLPEntity[entity] = newIndex;

	return newIt;
}

void DynamicGrid::OnEntityDestroy(LPEntity entity)
{
	auto it = indexbyLPEntity.find(entity);
	indexbyLPEntity.erase(it);
}
