#include "stdafx.h"
#include "Pathfinding.h"

Pathfinding::Pathfinding(Object* player, Object* enemy)
	: cols(32), rows(32), player(player), enemy(enemy)
{
	timer.Start();
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			grid[x][y] = nullptr;
		}
	}
}

Pathfinding::~Pathfinding()
{
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			delete grid[x][y];
		}
	}
}

void Pathfinding::Init()
{

	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			grid[x][y] = new Node();
		}
	}
}

void Pathfinding::DrawGrid()
{
	timer.Update();
	ResetPath();
	AStar();

	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			dx::XMFLOAT3 color;
			if (grid[i][j]->obstacle)
			{
				color = dx::XMFLOAT3(0, 0, 0);
			}
			else
			{
				if (grid[i][j]->openSet)
					color = dx::XMFLOAT3(0, 1, 0);
				else if (grid[i][j]->correctPath)
					color = dx::XMFLOAT3(0, 0, 1);
				else if (grid[i][j]->closedSet)
					color = dx::XMFLOAT3(1, 0, 0);
				else
					color = dx::XMFLOAT3(1, 1, 1);
			}
			
			box.DrawBox(dx::XMFLOAT3(grid[i][j]->pos.x * 5, 5, grid[i][j]->pos.y * 5), dx::XMFLOAT3(2, 2, 2), color);
		}
	}
}

void Pathfinding::AStar()
{
	timer.Restart();
	//dx::XMFLOAT3 enemyPos;
	//dx::XMStoreFloat3(&enemyPos, enemy->GetTransform().GetPosition());
	//Node* start = grid[(int)enemyPos.x][(int)enemyPos.z];
	//dx::XMFLOAT3 playerPos;
	//dx::XMStoreFloat3(&playerPos, player->GetTransform().GetPosition());
	//Node* end = grid[(int)playerPos.x][(int)playerPos.z];

	Node* start = grid[0][0];
	Node* end = grid[cols - 1][rows - 1];
	start->obstacle = false;
	end->obstacle = false;
	std::vector<Node*> openSet;
	std::vector<Node*> closedSet;
	openSet.push_back(start);
	grid[(int)start->pos.x][(int)start->pos.y]->openSet = true;
	bool pathFound = false;
	while (openSet.size() > 0)
	{
		Node* current = nullptr;
		
		std::make_heap(openSet.begin(), openSet.end(), Compare());
		current = openSet[0];
		openSet.erase(openSet.begin());

		if (current != nullptr && current->pos.x == end->pos.x && current->pos.y == end->pos.y)
		{
			pathFound = true;
			Node* temp = current;
			std::vector<Node*> bestPath;
			grid[(int)temp->pos.x][(int)temp->pos.y]->correctPath = true;
			bestPath.push_back(grid[(int)temp->pos.x][(int)temp->pos.y]);
			while (temp->previous != nullptr)
			{
				temp = temp->previous;
				grid[(int)temp->pos.x][(int)temp->pos.y]->correctPath = true;
				bestPath.push_back(grid[(int)temp->pos.x][(int)temp->pos.y]);
			}
			std::cout << "DONE!" << std::endl;
			
			
			//for (int i = 0; i < bestPath.size(); i++)
			//{
			//	std::cout << bestPath[i]->fCost << std::endl;
			//}
			std::cout << "Time it took: " << timer.GetSeconds() << std::endl;
			std::cout << "bestPath size: " << bestPath.size() << std::endl;
		}

		openSet.shrink_to_fit();
		grid[(int)current->pos.x][(int)current->pos.y]->openSet = false;
		closedSet.push_back(current);
		grid[(int)current->pos.x][(int)current->pos.y]->closedSet = true;

		for (int i = 0; i < current->neighbors.size(); i++)
		{
			if ((closedSet.empty() || std::find(closedSet.begin(), closedSet.end(), current->neighbors[i]) == closedSet.end()) 
				&& !current->neighbors[i]->obstacle)
			{
				int tempG = current->gCost + 1;

				bool newPath = false;
				if (std::find(openSet.begin(), openSet.end(), current->neighbors[i]) != openSet.end())
				{
					if (tempG < current->neighbors[i]->gCost)
					{
						current->neighbors[i]->gCost = tempG;
						newPath = true;
					}
				}
				else
				{
					current->neighbors[i]->gCost = tempG;
					openSet.push_back(current->neighbors[i]);
					grid[(int)current->neighbors[i]->pos.x][(int)current->neighbors[i]->pos.y]->openSet = true;
					newPath = true;
				}	
				if (newPath)
				{
					current->neighbors[i]->hCost = GetDistance(current->neighbors[i], end);
					current->neighbors[i]->fCost = current->neighbors[i]->gCost + current->neighbors[i]->hCost;
					current->neighbors[i]->previous = current;
				}
			}
		}
	}
	if (!pathFound)
		std::cout << "No path available" << std::endl;
}

void Pathfinding::AddNeighbors(Node* node)
{
	if (node->pos.x < (cols - 1))
		node->neighbors.push_back(grid[(int)node->pos.x + 1][(int)node->pos.y]);
	if (node->pos.x > 0)
		node->neighbors.push_back(grid[(int)node->pos.x - 1][(int)node->pos.y]);
	if (node->pos.y < (rows - 1))
		node->neighbors.push_back(grid[(int)node->pos.x][(int)node->pos.y + 1]);
	if (node->pos.y > 0)
		node->neighbors.push_back(grid[(int)node->pos.x][(int)node->pos.y - 1]);
	if (node->pos.x > 0 && node->pos.y > 0)
		node->neighbors.push_back(grid[(int)node->pos.x - 1][(int)node->pos.y - 1]);
	if (node->pos.x < cols - 1 && node->pos.y > 0)
		node->neighbors.push_back(grid[(int)node->pos.x + 1][(int)node->pos.y - 1]);
	if (node->pos.x > 0 && node->pos.y < rows - 1)
		node->neighbors.push_back(grid[(int)node->pos.x - 1][(int)node->pos.y + 1]);
	if (node->pos.x < cols - 1 && node->pos.y < rows - 1)
		node->neighbors.push_back(grid[(int)node->pos.x + 1][(int)node->pos.y + 1]);
}

void Pathfinding::AddNeighbors2(Node* node)
{
	for (int x = -1; x <= 1; x++)
	{
		for (int y = -1; y <= 1; y++)
		{
			if (x == 0 && y == 0)
				continue;

			int checkX = (int)node->pos.x + x;
			int checkY = (int)node->pos.y + y;

			if (checkX >= 0 && checkX < ((cols -1) * (rows - 1)) && checkY >= 0 && checkY < ((cols - 1) * (rows - 1)))
			{
				node->neighbors.push_back(grid[checkX][checkY]);
			}
		}
	}
}

int Pathfinding::GetDistance(Node* nodeA, Node* nodeB)
{
	return abs(nodeA->pos.x - nodeB->pos.x) + abs(nodeA->pos.y - nodeB->pos.y);
}

void Pathfinding::AddObstacles()
{
	srand(time(NULL));
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			int randNr = rand() % 100;
			if(randNr < 30)
				grid[x][y]->obstacle = true;
		}
	}
}

void Pathfinding::ResetPath()
{
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			grid[x][y]->pos = dx::XMFLOAT2(x, y);
			grid[x][y]->gCost = 0;
			grid[x][y]->hCost = 0;
			grid[x][y]->fCost = 0;
			grid[x][y]->obstacle = false;
			grid[x][y]->openSet = false;
			grid[x][y]->closedSet = false;
			grid[x][y]->correctPath = false;
			grid[x][y]->previous = nullptr;
			grid[x][y]->neighbors.clear();
		}
	}

	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			AddNeighbors(grid[x][y]);
		}
	}

	AddObstacles();
}
