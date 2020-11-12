#pragma once
#include "Object.h"

struct Grid
{
	dx::XMFLOAT2 buttomLeft;
	dx::XMFLOAT2 buttomRight;
	dx::XMFLOAT2 topRight;
	dx::XMFLOAT2 topLeft;
	dx::XMFLOAT2 position;
};

class GridMap
{
	public:
		GridMap(Object* object);
		~GridMap();
		void Init();
		bool CheckInGrid();
	private:
		Object* object;
		std::vector<Grid> grids;
		Grid oneGrid;
};