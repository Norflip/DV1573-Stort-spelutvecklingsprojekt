#pragma once
#include "Object.h"
#include "GameClock.h"
#include "PlayerComp.h"

class Pathfinding : public Component
{
	public:
		Pathfinding(PlayerComp* player);
		virtual ~Pathfinding();
		void Initialize();
		void Update(const float& deltaTime) override;
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
		void FollowPath();

		int cols;
		int rows;
		Node* grid[32][32];
		GameClock timer;
		PlayerComp* player;
		std::vector<Node*> correctPath;
		bool pathFound;
		float playerRadius;
		RigidBodyComponent* enemyRB;
		EnemyStatsComp* enemyStatsComp;
};