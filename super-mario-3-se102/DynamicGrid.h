#pragma once
#include "Grid.h"

//Spatial Grid for movable entities 
class DynamicGrid : public Grid
{
public:
	DynamicGrid(int numberOfColumns, int numberOfRows, const Utils::Dimension& cellSize);
	void Update();
	void AddToCell(LPEntity entity, const Utils::Vector2<int>& cellIndex) override;
	void UpdateEntityCellIndex(LPEntity entity);
private:
	std::unordered_map<LPEntity, Utils::Vector2<int>> IndexbyLPEntity;
};
typedef DynamicGrid* LPDynamicGrid;