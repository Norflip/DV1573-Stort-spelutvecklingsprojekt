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
		void PriorityQueue(int cost);

		int rows;
		float nodeSize;
		int objectOffset;
		Object* object;
		DShape box;
		std::priority_queue<int, int> maxHeap;
};