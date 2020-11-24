#pragma once
#include "Object.h"

struct Node
{
	int gCost = 0;
	int hCost = 0;
	int fCost = 0;
	int posX = 0;
	int posY = 0;
	bool openList = false;
	bool closedList = false;
	bool correctPath = false;
	std::vector<Node*> neighbors;
	Node* previous = nullptr;
	Node* parent = nullptr;
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
	Grid();
	~Grid();
	void Init();
	void DrawNodes();

private:
	void FindPath();
	std::vector<Node*> GetNeighbours(Node* node);
	int GetDistance(Node* nodeA, Node* nodeB);
	void RetracePath(Node* startNode, Node* endNode);
	
	void FindPath2();
	void AddNeighbors(Node* node);
	int Heuristic(Node* neighbor, Node* end);
	bool Includes(std::vector<Node*> checkVector, Node* node);

	void TestNodes();

	int rows;
	int columns;
	int gridSize;
	std::vector<Node*> allNodes;
	std::vector<Node*> openList;
	std::vector<Node*> closedList;
	std::vector<Node*> gridPath;
	std::vector<std::vector<Node*>> testGrid;
	DShape box;
};