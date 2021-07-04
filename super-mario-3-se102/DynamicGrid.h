#pragma once
#include "Grid.h"

//Spatial Grid for movable entities 
class DynamicGrid : public Grid
{
public:
	DynamicGrid(int numberOfColumns, int numberOfRows, const Utils::Dimension<float>& cellSize);
	void AddToCell(LPEntity entity, const Utils::Vector2<int>& cellIndex) override;
	void UpdateCells(const CellRange& range);
	EntitiesInCell::const_iterator UpdateEntityCellIndex(EntitiesInCell::const_iterator it);
private:
	std::unordered_map<LPEntity, Utils::Vector2<int>> indexbyLPEntity;
	void OnEntityDestroy(LPEntity entity);
};
typedef DynamicGrid* LPDynamicGrid;