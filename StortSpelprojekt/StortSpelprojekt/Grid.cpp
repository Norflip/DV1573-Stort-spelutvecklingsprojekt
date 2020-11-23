#include "stdafx.h"
#include "Grid.h"

Grid::Grid()
	: rows(8), columns(4), gridSize(64)
{
}

Grid::~Grid()
{
}

void Grid::Init()
{
	for (int x = 0; x < rows; x++)
	{
		for (int z = 0; z < rows; z++)
		{
			Node* temp = new Node;
			temp->posX = x;
			temp->posY = z;
			allNodes.push_back(temp);
		}
	}
	FindPath(dx::XMFLOAT2(0, 0), dx::XMFLOAT2(8, 8));
}

void Grid::DrawNodes()
{
	for (int i = 0; i < allNodes.size(); i++)
	{
		if (allNodes[i]->correctPath)
			box.DrawBox(dx::XMFLOAT3(allNodes[i]->posX * 4, 8, allNodes[i]->posY * 4), dx::XMFLOAT3(2, 2, 2), dx::XMFLOAT3(0, 1, 0));
		else
			box.DrawBox(dx::XMFLOAT3(allNodes[i]->posX * 4, 8, allNodes[i]->posY * 4), dx::XMFLOAT3(2, 2, 2), dx::XMFLOAT3(1, 0, 0));
	}
}

void Grid::FindPath(dx::XMFLOAT2 startPos, dx::XMFLOAT2 endPos)
{
	Node* startNode = new Node;
	Node* endNode = new Node;
	int erasePos = 0;
	//startNode->posX = startPos.x;
	//startNode->posY = startPos.y;
	startNode = allNodes[0];
	//endNode->posX = endPos.x;
	//endNode->posY = endPos.y;
	endNode = allNodes[4];
	openList.push_back(startNode);
	while (openList.size() > 0)
	{
		std::cout << "Open list: " << openList.size() << std::endl;
		std::cout << "Closed list: " << closedList.size() << std::endl;
		Node* currentNode = openList[0];
		for (int i = 1; i < openList.size(); i++)
		{
			if (openList[i]->fCost < currentNode->fCost || openList[i]->fCost == currentNode->fCost && openList[i]->hCost < currentNode->hCost)
			{
				currentNode = openList[i];
				erasePos = i;
			}
		}

		openList.erase(openList.begin() + erasePos);
		openList.shrink_to_fit();
		closedList.push_back(currentNode);

		if (currentNode == endNode)
		{
			std::cout << "Retracing path" << std::endl;
			RetracePath(startNode, endNode);
			return;
		}

		for (auto neighbour : GetNeighbours(currentNode))
		{
			if (std::find(closedList.begin(), closedList.end(), neighbour) != closedList.end())
			{
				continue;
			}
			int newCostToNeighbour = currentNode->gCost + GetDistance(currentNode, neighbour);
			if (newCostToNeighbour < neighbour->gCost || std::find(openList.begin(), openList.end(), neighbour) == openList.end())
			{
				neighbour = new Node;
				neighbour->gCost = newCostToNeighbour;
				neighbour->hCost = GetDistance(neighbour, endNode);
				neighbour->fCost = neighbour->gCost + neighbour->hCost;
				neighbour->parent = currentNode;
				if (std::find(openList.begin(), openList.end(), neighbour) == openList.end())
				{
					openList.push_back(neighbour);
				}
			}
		}
	}
	RetracePath(startNode, endNode);
}

std::vector<Node*> Grid::GetNeighbours(Node* node)
{
	std::vector<Node*> neighbours;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}

			int checkX = node->posX + x;
			int checkY = node->posY + y;

			if (checkX >= 0 && checkX < gridSize && checkY >= 0 && checkY < gridSize)
			{
				Node* neighbour = new Node;
				neighbour->posX = checkX;
				neighbour->posY = checkY;
				neighbours.push_back(neighbour);
			}
		}
	}
	return neighbours;
}

int Grid::GetDistance(Node* nodeA, Node* nodeB)
{
	int distX = abs(nodeA->posX - nodeB->posX);
	int distY = abs(nodeA->posY - nodeB->posY);

	if (distX > distY)
		return 14 * distY + 10 * (distX - distY);

	return 14 * distX + 10 * (distY - distX);
}

void Grid::RetracePath(Node* startNode, Node* endNode)
{
	std::vector<Node*> path;
	Node* currentNode = new Node;
	currentNode = endNode;

	while (currentNode != startNode)
	{
		path.push_back(currentNode);
		allNodes[currentNode->posX + currentNode->posY]->correctPath = true;
		currentNode = currentNode->parent;
	}

	std::reverse(path.begin(), path.end());

	for (int i = 0; i < path.size(); i++)
		gridPath.push_back(path[i]);
}