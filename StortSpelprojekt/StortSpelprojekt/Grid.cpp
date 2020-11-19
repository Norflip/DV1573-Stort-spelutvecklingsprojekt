#include "stdafx.h"
#include "Grid.h"

Grid::Grid(Object* object)
	: object(object), rows(8), nodeSize(4.0f), objectOffset(5), gridSize(32)
{
	
}

Grid::~Grid()
{
}

void Grid::Init()
{
}

void Grid::DrawGrid()
{
	std::cout << gridPath.size() << std::endl;
	bool checkNode = false;
	for (int i = 0; i < allNodes.size(); i++)
	{
		for (int j = 0; j < gridPath.size(); j++)
		{
			if (allNodes[i]->pos.x == gridPath[j]->pos.x && allNodes[i]->pos.y == gridPath[j]->pos.y)
			{
				checkNode = true;
				break;
			}
			else
			{
				checkNode = false;
			}
		}
		if (checkNode)
			box.DrawBox(dx::XMFLOAT3((allNodes[i]->pos.x * nodeSize) + 5, 4, (allNodes[i]->pos.y * nodeSize) + 5), dx::XMFLOAT3(2, 2, 2), dx::XMFLOAT3(0, 1, 0));
		else
			box.DrawBox(dx::XMFLOAT3((allNodes[i]->pos.x * nodeSize) + 5, 4, (allNodes[i]->pos.y * nodeSize) + 5), dx::XMFLOAT3(2, 2, 2), dx::XMFLOAT3(1, 0, 0));
		checkNode = false;
	}

	//for (int i = 0; i < allNodes.size(); i++)
	//{
	//	box.DrawBox(dx::XMFLOAT3((allNodes[i].pos.x * nodeSize) + 5, 4, (allNodes[i].pos.y * nodeSize) + 5), dx::XMFLOAT3(2, 2, 2), dx::XMFLOAT3(1, 0, 0));
	//}
}

void Grid::FindPath(dx::XMFLOAT2 startPos, dx::XMFLOAT2 endPos)
{
	Node* startNode = new Node;
	Node* endNode = new Node;
	Node* currentNode = new Node;
	int erasePos = 0;

	startNode->pos.x = startPos.x;
	startNode->pos.y = startPos.y;
	endNode->pos.x = endPos.x;
	endNode->pos.y = endPos.y;

	openList.push_back(startNode);
	while (openList.size() > 0)
	{
		currentNode = openList[0];
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
		if (currentNode->pos.x == endNode->pos.x && currentNode->pos.y == endNode->pos.y)
		{
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
				neighbour->parent = currentNode;
				if (std::find(openList.begin(), openList.end(), neighbour) == openList.end())
				{
					openList.push_back(neighbour);
				}
			}
		}
	}
}

void Grid::CalculatePath()
{
	//dx::XMFLOAT3 objectPos;
	//dx::XMStoreFloat3(&objectPos, object->GetTransform().GetPosition());

	//int gridPosX = std::round((objectPos.x - objectOffset) / nodeSize);
	//int gridPosZ = std::round((objectPos.z - objectOffset) / nodeSize);
	//int gridPosZ = (std::round((objectPos.z - objectOffset) / nodeSize) * rows) + 1;
	//int gridPos = gridPosX * gridPosZ;
	//dx::XMFLOAT2 startPos = dx::XMFLOAT2(gridPosX, gridPosZ);
	dx::XMFLOAT2 startPos = dx::XMFLOAT2(0, 0);
	dx::XMFLOAT2 endPos = dx::XMFLOAT2(2, 2);
	FindPath(startPos, endPos);

	for (int x = 0; x < rows; x++)
	{
		for (int z = 0; z < rows; z++)
		{
			Node* temp = new Node;
			temp->pos.x = x;
			temp->pos.y = z;
			allNodes.push_back(temp);
		}
	}
}

std::vector<Node*> Grid::GetNeighbours(Node* node)
{
	std::vector<Node*> neighbours;
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			if ((x + y) == 0)
				continue;
			int checkX = node->pos.x + x;
			int checkY = node->pos.y + y;

			if (checkX >= 0 && checkX < gridSize && checkY >= 0 && checkY < gridSize)
			{
				Node* neighbour = new Node;
				neighbour->pos.x = checkX;
				neighbour->pos.y = checkY;
				neighbours.push_back(neighbour);
			}
		}
	}
	return neighbours;
}

int Grid::GetDistance(Node* nodeA, Node* nodeB)
{
	int distX = abs(nodeA->pos.x - nodeB->pos.x);
	int distY = abs(nodeA->pos.y - nodeB->pos.y);

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
		currentNode = currentNode->parent;
	}
	
	std::reverse(path.begin(), path.end());
	
	for (int i = 0; i < path.size(); i++)
		gridPath.push_back(path[i]);
}
