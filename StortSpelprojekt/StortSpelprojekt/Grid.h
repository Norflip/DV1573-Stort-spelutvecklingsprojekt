#pragma once
#include "Object.h"

struct Node
{
	int gCost;
	int hCost;
	int fCost;
	int posX;
	int posY;
	struct Node* parent;
	bool operator==(const Node& other)
	{
		return posX == other.posX && posY == other.posY;
	}
	bool operator!=(const Node& other)
	{
		return posX != other.posX && posY != other.posY;
	}
};

class Grid
{
	public:
		Grid(Object* object);
		~Grid();
		void Init();
		void DrawGrid();
	private:
		void FindPath(dx::XMFLOAT2 startPos, dx::XMFLOAT2 endPos);
		std::vector<Node> GetNeighbours(Node node);
		int GetDistance(Node nodeA, Node nodeB);
		void RetracePath(Node startNode, Node endNode);

		int rows;
		float nodeSize;
		int objectOffset;
		int gridSize;
		Object* object;
		DShape box;
		std::vector<Node> openList;
		std::vector<Node> closedList;
		std::vector<Node> gridPath;
		std::vector<Node> allNodes;
};