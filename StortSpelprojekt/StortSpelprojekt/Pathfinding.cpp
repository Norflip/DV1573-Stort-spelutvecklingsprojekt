#include "stdafx.h"
#include "Pathfinding.h"

Pathfinding::Pathfinding()
	: cols(5), rows(5)
{
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			grid[x][y] = nullptr;
		}
	}
}

void Pathfinding::Init()
{
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			grid[x][y] = new NodeClass(x, y);
		}
	}
	for (int x = 0; x < cols; x++)
	{
		for (int y = 0; y < rows; y++)
		{
			AddNeighbors(grid[x][y]);
		}
	}
	AStar();
}

void Pathfinding::DrawGrid()
{
	for (int i = 0; i < cols; i++)
	{
		for (int j = 0; j < rows; j++)
		{
			dx::XMFLOAT3 color;
			if (grid[i][j]->GetOpenSet())
				color = dx::XMFLOAT3(0, 1, 0);
			else if (grid[i][j]->GetClosedSet())
				color = dx::XMFLOAT3(1, 0, 0);
			else if (grid[i][j]->GetCorrectPath())
				color = dx::XMFLOAT3(0, 0, 1);
			else
				color = dx::XMFLOAT3(1, 1, 1);

			box.DrawBox(dx::XMFLOAT3(grid[i][j]->GetX() * 5, 5, grid[i][j]->GetY() * 5), dx::XMFLOAT3(2, 2, 2), color);
		}
	}
}

void Pathfinding::AStar()
{
	NodeClass* start = grid[0][0];
	NodeClass* end = grid[cols - 1][rows - 1];
	openSet.push_back(start);
	grid[start->GetX()][start->GetY()]->SetOpenSet(true);
	while (openSet.size() > 0)
	{
		int winner = 0;
		for (int i = 0; i < openSet.size(); i++)
		{
			if (openSet[i]->GetFCost() < openSet[winner]->GetFCost())
			{
				winner = i;
			}
		}
		NodeClass* current = openSet[winner];

		if (current->GetX() == end->GetX() && current->GetY() == end->GetY())
		{
			NodeClass* temp = current;
			grid[temp->GetX()][temp->GetY()]->SetCorrectPath(true);
			while (temp->GetPrevious() != nullptr)
			{
				temp = temp->GetPrevious();
				grid[temp->GetX()][temp->GetY()]->SetCorrectPath(true);
			}
			std::cout << "DONE!" << std::endl;
		}

		openSet.erase(openSet.begin() + winner);
		openSet.shrink_to_fit();
		grid[current->GetX()][current->GetY()]->SetOpenSet(false);
		closedSet.push_back(current);
		grid[current->GetX()][current->GetY()]->SetClosedSet(true);

		for (int i = 0; i < current->GetNeighbors().size(); i++)
		{
			if (closedSet.empty() || std::find(closedSet.begin(), closedSet.end(), current->GetNeighbors()[i]) == closedSet.end())
			{
				int tempG = current->GetGCost() + 1;
				if (std::find(openSet.begin(), openSet.end(), current->GetNeighbors()[i]) != openSet.end())
				{
					if (tempG < current->GetNeighbors()[i]->GetGCost())
					{
						current->GetNeighbors()[i]->SetGCost(tempG);
					}
				}
				else
				{
					current->GetNeighbors()[i]->SetGCost(tempG);
					openSet.push_back(current->GetNeighbors()[i]);
					grid[current->GetNeighbors()[i]->GetX()][current->GetNeighbors()[i]->GetY()]->SetOpenSet(true);
				}	
				current->GetNeighbors()[i]->SetHCost(GetDistance(current->GetNeighbors()[i], end));
				current->GetNeighbors()[i]->SetFCost(current->GetNeighbors()[i]->GetGCost() + current->GetNeighbors()[i]->GetHCost());
				current->GetNeighbors()[i]->SetPrevious(current);
			}
		}
	}
}

void Pathfinding::AddNeighbors(NodeClass* node)
{
	if (node->GetX() < (cols - 1))
		node->AddNeighbor(grid[node->GetX() + 1][node->GetY()]);
	if (node->GetX() > 0)
		node->AddNeighbor(grid[node->GetX() - 1][node->GetY()]);
	if (node->GetY() < (rows - 1))
		node->AddNeighbor(grid[node->GetX()][node->GetY() + 1]);
	if (node->GetY() > 0)
		node->AddNeighbor(grid[node->GetX()][node->GetY() - 1]);
}

int Pathfinding::GetDistance(NodeClass* nodeA, NodeClass* nodeB)
{
	return abs(nodeA->GetX() - nodeB->GetX()) + abs(nodeA->GetY() - nodeB->GetY());
}
