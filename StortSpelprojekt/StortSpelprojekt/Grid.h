#pragma once
#include "Object.h"

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