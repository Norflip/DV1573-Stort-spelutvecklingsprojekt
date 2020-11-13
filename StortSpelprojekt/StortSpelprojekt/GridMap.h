#pragma once
#include "Object.h"

struct Grid
{
	
};

class GridMap
{
	public:
		GridMap(Object* object);
		~GridMap();
		void Init();
		bool CheckInGrid();
	private:
		int rows;
		float gridSize;
		int objectOffset;
		int gridOffset;
		Object* object;
		std::vector<Grid> grids;
};