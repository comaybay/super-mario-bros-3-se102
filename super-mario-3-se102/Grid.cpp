#include "Grid.h"
#include "Event.h"
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
	cells[int(cellIndex.y * numOfCols + cellIndex.x)]->push_back(entity);
	entity->GetDestroyEvent()->Subscribe(this, &Grid::OnEntityDestroy);
}

LPConstEntitiesInCell Grid::EntitiesAt(const Vector2<int>& cellIndex)
{
	return cells[int(cellIndex.y * numOfCols + cellIndex.x)];
}

void Grid::ForEachEntityIn(const CellRange& range, std::function<void(LPEntity)> handler)
{
	for (int y = 0; y < range.rowSpan; y++)
		for (int x = 0; x < range.colSpan; x++) {
			LPConstEntitiesInCell entities = EntitiesAt(range.startCellIndex + Vector2<int>(x, y));
			for (LPEntity entity : *entities)
				handler(entity);
		};
}

Vector2<int> Grid::GetCellIndexAtPoint(const Vector2<float>& point)
{
	Vector2<int> position(int(point.x / cellSize.width), int(point.y / cellSize.height));

	//if position is out of range, clip it to the closet edge of grid
	position.x = Clip(position.x, 0, numOfCols - 1);
	position.y = Clip(position.y, 0, numOfRows - 1);

	return position;
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

void Grid::OnEntityDestroy(LPEntity entity)
{
	Vector2<int> index = GetCellIndexAtPoint(entity->GetPosition());
	cells[index.y * numOfCols + index.x]->remove(entity);
}

