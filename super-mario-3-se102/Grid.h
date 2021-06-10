#pragma once
#include <list>
#include "Entity.h"

//reference: https://gameprogrammingpatterns.com/spatial-partition.html
class Grid
{
public:
	Grid(int numberOfCollumns, int numberOfRows);
	void Add(LPEntity entity, int column, int row);
	const std::list<LPEntity>* EntitiesAt(LPEntity entity, int column, int row);
protected:
	//each cell contains a list of entities
	std::vector<std::list<LPEntity>*> cells;
	int numOfRows;
	int numOfCols;
};

