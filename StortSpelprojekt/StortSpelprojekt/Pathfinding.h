#pragma once
#include "Object.h"
#include "GameClock.h"
#include "PlayerComp.h"

constexpr int GRIDSIZE = 32;

class Pathfinding : public Component
{
	public:
		Pathfinding(PlayerComp* player);
		virtual ~Pathfinding();
		void Initialize();
		void Update(const float& deltaTime) override;
		void SetPlayer(PlayerComp* playerComp);
	private:
		struct Node
		{
			dx::XMFLOAT2 pos = { 0,0 };
			int gCost = 0;					//cost to neighbor node
			int hCost = 0;					//cost from current node to endnode	
			int fCost = 0;					//total cost of h and g
			bool obstacle = false;
			bool openSet = false;
			bool closedSet = false;
			bool correctPath = false;
			Node* previous = nullptr;		//trace back to previous node to find the best path
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
		void FollowPath();
		void AddObstaclesToNeighbors(Node* node);

		int cols;
		int rows;
		Node* grid[GRIDSIZE][GRIDSIZE];
		GameClock timer;
		PlayerComp* player;
		std::vector<Node*> correctPath;
		bool pathFound;
		float playerRadius;
		RigidBodyComponent* enemyRB;
		EnemyStatsComp* enemyStatsComp;
		float length;
		Node* end;
};