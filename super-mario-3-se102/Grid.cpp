#include "Grid.h"

Grid::Grid(int numberOfColumns, int numberOfRows, Utils::Dimension cellSize)
	: numOfCols(numberOfColumns), numOfRows(numberOfRows), cellSize(cellSize),
	cells(std::vector<LPEntitiesInCell>(numberOfRows* numberOfColumns))
{
	for (auto& cell : cells)
		cell = new std::list<LPEntity>();

}


void Grid::Add(LPEntity entity, const Utils::Vector2& cellIndex)
{
	cells[cellIndex.y * cellIndex.x + cellIndex.x]->push_back(entity);
}

const LPEntitiesInCell Grid::EntitiesAt(const Utils::Vector2& cellIndex)
{
	return cells[cellIndex.y * cellIndex.x + cellIndex.x];
}

Utils::Vector2 Grid::GetCellIndexAtPoint(const Utils::Vector2& point)
{
	return Utils::Vector2(point.x / cellSize.width, point.y / cellSize.height);
}

