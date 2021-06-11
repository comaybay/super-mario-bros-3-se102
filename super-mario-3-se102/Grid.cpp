#include "Grid.h"
using namespace Utils;

CellRange::CellRange(const Utils::Vector2<int>& startCellIndex, int colSpan, int rowSpan)
	: startCellIndex(startCellIndex), colSpan(colSpan), rowSpan(rowSpan)
{}

Grid::Grid(int numberOfColumns, int numberOfRows, const Dimension& cellSize)
	: numOfCols(numberOfColumns), numOfRows(numberOfRows), cellSize(cellSize),
	cells(std::vector<LPEntitiesInCell>(numberOfRows* numberOfColumns))
{
	for (auto& cell : cells)
		cell = new std::list<LPEntity>();
}

Grid::~Grid()
{
	for (LPEntitiesInCell cell : cells)
		delete cell;
}

void Grid::AddToCell(LPEntity entity, const Vector2<int>& cellIndex)
{
	cells[int(cellIndex.y * cellIndex.x + cellIndex.x)]->push_back(entity);
}

LPConstEntitiesInCell Grid::EntitiesAt(const Vector2<int>& cellIndex)
{
	return cells[int(cellIndex.y * cellIndex.x + cellIndex.x)];
}

Vector2<int> Grid::GetCellIndexAtPoint(const Vector2<float>& point)
{
	return Vector2<int>(int(point.x / cellSize.width), int(point.y / cellSize.height));
}

CellRange Grid::GetCellRangeFromRectangle(const Vector2<float>& position, const Dimension& dim)
{
	Vector2<int> topLeftPos = GetCellIndexAtPoint(position);
	Vector2<int> bottomRightPos = GetCellIndexAtPoint(Vector2<float>(position.x + dim.width, position.y + dim.height));

	Vector2<int> difference = bottomRightPos - topLeftPos;
	int colSpan = difference.x + 1;
	int rowSpan = difference.y + 1;

	return CellRange(topLeftPos, colSpan, rowSpan);
}

