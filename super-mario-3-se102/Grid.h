#pragma once
#include <list>
#include <unordered_map>
#include "Entity.h"
#include "Utils.h"

typedef std::list<LPEntity>* LPEntitiesInCell;
//reference: https://gameprogrammingpatterns.com/spatial-partition.html
//Basic Spatial Grid, treat each entity at a point, do not take into account it's size
class Grid
{
public:
	Grid(int numberOfCollumns, int numberOfRows, Utils::Dimension cellSize);
	virtual void Add(LPEntity entity, const Utils::Vector2& cellIndex);
	const LPEntitiesInCell EntitiesAt(const Utils::Vector2& cellIndex);
	Utils::Vector2 GetCellIndexAtPoint(const Utils::Vector2& point);
protected:
	//each cell contains a list of entities
	std::vector<LPEntitiesInCell> cells;
	int numOfRows;
	int numOfCols;
	Utils::Dimension cellSize;
};

