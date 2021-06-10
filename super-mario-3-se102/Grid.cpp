#include "Grid.h"

Grid::Grid(int numberOfColumns, int numberOfRows)
	: numOfCols(numberOfColumns), numOfRows(numberOfRows),
	cells(std::vector<std::list<LPEntity>*>(numberOfRows* numberOfColumns))
{
	for (auto& cell : cells)
		cell = new std::list<LPEntity>();

}

void Grid::Add(LPEntity entity, int column, int row)
{
	cells[row * column + column]->push_back(entity);
}

const std::list<LPEntity>* Grid::EntitiesAt(LPEntity entity, int column, int row)
{
	return cells[column * row + column];
}

