#include "stdafx.h"
#include "Grid.h"

Grid::Grid()
	: rows(5), columns(5), gridSize(5)
{
}

Grid::~Grid()
{
}

void Grid::Init()
{
	for (int z = 0; z < columns; z++)
	{
		testGrid.push_back(std::vector<Node*>());
		for (int x = 0; x < rows; x++)
		{
			Node* temp = new Node;
			temp->posX = x;
			temp->posY = z;
			allNodes.push_back(temp);
			testGrid[z].push_back(temp);
		}
	}

	for (int i = 0; i < allNodes.size(); i++)
	{
		AddNeighbors(allNodes[i]);
	}
	//TestNodes();
	FindPath();
	//FindPath2();
}

void Grid::DrawNodes()
{
	for (int i = 0; i < allNodes.size(); i++)
	{
		dx::XMFLOAT3 color;
		if (allNodes[i]->openList)
			color = dx::XMFLOAT3(0, 1, 0);
		else if (allNodes[i]->closedList)
			color = dx::XMFLOAT3(1, 0, 0);
		else if (allNodes[i]->correctPath)
			color = dx::XMFLOAT3(0, 0, 1);
		else
			color = dx::XMFLOAT3(1, 1, 1);

		box.DrawBox(dx::XMFLOAT3(allNodes[i]->posX * 4, 5, allNodes[i]->posY * 5), dx::XMFLOAT3(2, 2, 2), color);
	}
}

void Grid::FindPath()
{
	Node* startNode = new Node;
	Node* endNode = new Node;
	int erasePos = 0;
	startNode = allNodes[0];
	endNode = allNodes[12];
	openList.push_back(startNode);
	allNodes[startNode->posX + (startNode->posY * rows)]->openList = true;
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
		allNodes[currentNode->posX + (currentNode->posY * rows)]->openList = false;
		closedList.push_back(currentNode);
		allNodes[currentNode->posX + (currentNode->posY * rows)]->closedList = true;

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
					allNodes[neighbour->posX + (neighbour->posY * rows)]->openList = true;
				}
			}
		}
	}
}

std::vector<Node*> Grid::GetNeighbours(Node* node)
{
	std::vector<Node*> neighbours;
	for (int x = -1 + node->posX; x <= 1 + node->posX; x++)
	{
		for (int y = -1 + node->posY; y <= 1 + node->posY; y++)
		{
			if (x == node->posX && y == node->posY)
			{
				continue;
			}

			int checkX = x;
			int checkY = y;

			if (checkX >= 0 && checkX < gridSize && checkY >= 0 && checkY < gridSize)
			{
				neighbours.push_back(allNodes[checkX + (checkY * rows)]);
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
		allNodes[currentNode->posX + (currentNode->posY * rows)]->correctPath = true;
		currentNode = currentNode->parent;
	}

	std::reverse(path.begin(), path.end());

	for (int i = 0; i < path.size(); i++)
		gridPath.push_back(path[i]);
}

void Grid::FindPath2()
{
	Node* startNode = new Node;
	Node* endNode = new Node;
	startNode = allNodes[0];
	endNode = allNodes[12];
	openList.push_back(startNode);
	allNodes[startNode->posX + (startNode->posY * rows)]->openList = true;
	while(openList.size() > 0)
	{
		int winner = 0;
		for (int i = 0; i < openList.size(); i++)
		{
			if (openList[i]->fCost < openList[winner]->fCost)
				winner = i;
		}

		Node* current = openList[winner];

		if (current == endNode)
		{
			Node* temp = current;
			gridPath.push_back(temp);
			allNodes[temp->posX + (temp->posY * rows)]->correctPath = true;
			while (temp->previous != nullptr)
			{
				gridPath.push_back(temp->previous);
				temp = temp->previous;
				allNodes[temp->posX + (temp->posY * rows)]->correctPath = true;
			}

			std::cout << "Done" << std::endl;
		}
			
		openList.erase(openList.begin() + winner);
		openList.shrink_to_fit();
		allNodes[current->posX + (current->posY * rows)]->openList = false;
		closedList.push_back(current);
		allNodes[current->posX + (current->posY * rows)]->closedList = true;

		std::vector<Node*> neighbors;
		for(int i = 0; i < current->neighbors.size(); i++)
		{
			neighbors.push_back(current->neighbors[i]);
		}
		for (int i = 0; i < neighbors.size(); i++)
		{
			Node* neighbor = neighbors[i];

			if (!Includes(closedList, neighbor))
			{
				int tempG = current->gCost + 1;
				if (Includes(openList, neighbor))
				{
					if (tempG < neighbor->gCost)
					{
						neighbor->gCost = tempG;
					}
				}
				else
				{
					neighbor->gCost = tempG;
					openList.push_back(neighbor);
					allNodes[neighbor->posX + (neighbor->posY * rows)]->openList = true;
				}
				neighbor->hCost = Heuristic(neighbor, endNode);
				neighbor->fCost = neighbor->gCost + neighbor->hCost;
				neighbor->previous = current;
			}
		}
	}
	std::cout << "Open list: " << openList.size() << std::endl;
	std::cout << "Closed list: " << closedList.size() << std::endl;
}

void Grid::AddNeighbors(Node* node)
{
	if(node->posX < rows - 1)
		node->neighbors.push_back(allNodes[(node->posX + 1) + (node->posY * rows)]);
	if (node->posX > 0)
		node->neighbors.push_back(allNodes[(node->posX - 1) + (node->posY * rows)]);
	if (node->posY < rows - 1)
		node->neighbors.push_back(allNodes[node->posX + (node->posY * (rows + 1))]);
	if (node->posY > 0)
		node->neighbors.push_back(allNodes[node->posX + (node->posY * (rows - 1))]);
}

int Grid::Heuristic(Node* neighbor, Node* end)
{
	//std::list<int> myList;
	//myList.push_back(neighbor->posX + (neighbor->posY * rows));
	//myList.push_back(end->posX + (end->posY * rows));
	//std::list<int>::iterator first = myList.begin();
	//std::list<int>::iterator last = myList.end();

	//return std::distance(first, last);

	return abs(neighbor->posX - end->posX) + abs(neighbor->posY - end->posY);
}

bool Grid::Includes(std::vector<Node*> checkVector, Node* node)
{
	for (int i = 0; i < checkVector.size(); i++)
		if (checkVector[i] == node)
			return true;
	return false;
}

void Grid::TestNodes()
{
	for (int i = 0; i < 4; i++)
	{
		openList.push_back(allNodes[i]);
		Node* currentNode = openList[0];
		int eraseNode = 0;
		openList.erase(openList.begin() + eraseNode);
		openList.shrink_to_fit();
		closedList.push_back(currentNode);

		allNodes[closedList[i]->posX + (closedList[i]->posY * rows)]->correctPath = true;
	}

	for (int i = 0; i < allNodes.size(); i++)
	{
		if (allNodes[i]->correctPath)
			std::cout << "List pos: " << i << " , node pos x: " << allNodes[i]->posX << " , y: " << allNodes[i]->posY << std::endl;
	}
	std::cout << "Open list size: " << openList.size() << std::endl;
}
