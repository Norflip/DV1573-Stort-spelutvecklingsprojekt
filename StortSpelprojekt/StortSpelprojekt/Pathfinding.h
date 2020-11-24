#pragma once
#include "Object.h"
#include"NodeClass.h"

class Pathfinding
{
	public:
		Pathfinding();
		~Pathfinding() {}
		void Init();
		void DrawGrid();
	private:
		void AStar();
		void AddNeighbors(NodeClass* node);
		int GetDistance(NodeClass* nodeA, NodeClass* nodeB);

		int cols;
		int rows;
		std::vector<NodeClass*> openSet;
		std::vector<NodeClass*> closedSet;
		NodeClass* grid[32][32];
		DShape box;
};