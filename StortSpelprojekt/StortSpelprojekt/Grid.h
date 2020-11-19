#pragma once
#include "Object.h"

struct Node
{
	int gCost;
	int hCost;
	int fCost;
	dx::XMFLOAT2 pos;
	Node* parent;
	bool operator==(const Node& other)
	{
		return pos.x == other.pos.x && pos.y == other.pos.y;
	}
	bool operator!=(const Node& other)
	{
		return pos.x != other.pos.x && pos.y != other.pos.y;
	}
};

class Grid
{
	public:
		Grid(Object* object);
		~Grid();
		void Init();
		void CalculatePath();
		void DrawGrid();
	private:
		void FindPath(dx::XMFLOAT2 startPos, dx::XMFLOAT2 endPos);
		std::vector<Node*> GetNeighbours(Node* node);
		int GetDistance(Node* nodeA, Node* nodeB);
		void RetracePath(Node* startNode, Node* endNode);

		int rows;
		float nodeSize;
		int objectOffset;
		int gridSize;
		Object* object;
		DShape box;
		std::vector<Node*> openList;
		std::vector<Node*> closedList;
		std::vector<Node*> gridPath;
		std::vector<Node*> allNodes;
};