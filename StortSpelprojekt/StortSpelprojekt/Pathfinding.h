#pragma once
#include "Object.h"
#include "GameClock.h"

class Pathfinding
{
	public:
		Pathfinding();
		~Pathfinding();
		void Init();
		void DrawGrid();
	private:
		struct Node
		{
			dx::XMFLOAT2 pos = { 0,0 };
			int gCost = 0;
			int hCost = 0;
			int fCost = 0;
			bool obstacle = false;
			bool openSet = false;
			bool closedSet = false;
			bool correctPath = false;
			Node* previous = nullptr;
			std::vector<Node*> neighbors;
		};
		struct Compare
		{
			bool operator()(const Node* nodeA, const Node* nodeB)
			{
				return nodeA->fCost > nodeB->fCost;
			}
		};

		void AStar(bool testHeap);
		void AddNeighbors(Node* node);
		void AddNeighbors2(Node* node);
		int GetDistance(Node* nodeA, Node* nodeB);
		void AddObstacles();
		void ResetPath();
		void TestReset();

		int cols;
		int rows;
		Node* grid[150][150];
		DShape box;
		GameClock timer;
		bool usingHeap = false;
};