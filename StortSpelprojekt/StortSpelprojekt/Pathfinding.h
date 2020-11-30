#pragma once
#include "Object.h"
#include "GameClock.h"

class Pathfinding
{
	public:
		Pathfinding(Object* player, Object* enemy);
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

		void AStar();
		void AddNeighbors(Node* node);
		void AddNeighbors2(Node* node);
		int GetDistance(Node* nodeA, Node* nodeB);
		void AddObstacles();
		void ResetPath();

		int cols;
		int rows;
		Node* grid[32][32];
		DShape box;
		GameClock timer;
		Object* player;
		Object* enemy;
};