#pragma once
#include "Grid.h"

//Spatial Grid for movable entities 
class DynamicGrid : public Grid
{
public:
	DynamicGrid(int numberOfColumns, int numberOfRows, const Utils::Dimension& cellSize);
	void Update();
	void AddToCell(LPEntity entity, const Utils::Vector2& cellIndex) override;
private:
	void UpdateEntityCellIndex(LPEntity entity);
	std::unordered_map<LPEntity, Utils::Vector2> IndexbyLPEntity;
};
typedef DynamicGrid* LPDynamicGrid;