#pragma once
#include "Object.h"

struct Node
{
	int gCost = 0;
	int hCost = 0;
	int fCost = 0;
	int posX;
	int posY;
	bool correctPath = false;
	Node* parent;
	bool operator==(const Node& other)
	{
		return gCost == other.gCost && hCost == other.hCost && parent == other.parent &&
			posX == other.posX && posY == other.posY;
	}
	bool operator!=(const Node& other)
	{
		return gCost != other.gCost && hCost != other.hCost && parent != other.parent &&
			posX != other.posX && posY != other.posY;
	}
};

class Grid
{
public:
	Grid();
	~Grid();
	void Init();
	void DrawNodes();

private:
	void FindPath(dx::XMFLOAT2 startPos, dx::XMFLOAT2 endPos);
	std::vector<Node*> GetNeighbours(Node* node);
	int GetDistance(Node* nodeA, Node* nodeB);
	void RetracePath(Node* startNode, Node* endNode);

	int rows;
	int columns;
	int gridSize;
	std::vector<Node*> allNodes;
	std::vector<Node*> openList;
	std::vector<Node*> closedList;
	std::vector<Node*> gridPath;
	DShape box;
};