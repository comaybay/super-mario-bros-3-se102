#pragma once
#include <list>
#include <unordered_map>
#include "Entity.h"
#include "Utils.h"

struct CellRange {
	Utils::Vector2<int> index;
	Utils::Vector2<int> span;
	CellRange(const Utils::Vector2<int>& cellIndex, const Utils::Vector2<int>& cellSpan);
};

typedef std::list<LPEntity> EntitiesInCell;
typedef EntitiesInCell* LPEntitiesInCell;
typedef const EntitiesInCell* LPConstEntitiesInCell;
//reference: https://gameprogrammingpatterns.com/spatial-partition.html
//Basic Spatial Grid, treat each entity at a point, do not take into account it's size
class Grid
{
public:
	Grid(int numberOfColumns, int numberOfRows, const Utils::Dimension& cellSize);
	~Grid();
	virtual void AddToCell(LPEntity entity, const Utils::Vector2<int>& cellIndex);
	LPConstEntitiesInCell EntitiesAt(const Utils::Vector2<int>& cellIndex);
	void ForEachEntityIn(const CellRange& range, std::function<void(LPEntity)> handler);
	void ForEachEntity(std::function<void(LPEntity)> handler);
	Utils::Vector2<int> GetCellIndexAtPoint(const Utils::Vector2<float>& point);
	CellRange GetCellRangeFromRectangle(const Utils::Vector2<float>& position, const Utils::Dimension& dim);
	void OnEntityDestroy(LPEntity entity);
protected:
	//each cell contains a list of entities
	std::vector<LPEntitiesInCell> cells;
	int numOfRows;
	int numOfCols;
	Utils::Dimension cellSize;
};
typedef Grid* LPGrid;

