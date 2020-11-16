#pragma once
#include "Object.h"

struct aStar
{
	int nextNode;
	int endNode;
	int totalCost;
};

class Grid
{
	public:
		Grid(Object* object);
		~Grid();
		void Init();
		bool CheckInGrid();
	private:
		int rows;
		float nodeSize;
		int objectOffset;
		Object* object;
		DShape box;
};