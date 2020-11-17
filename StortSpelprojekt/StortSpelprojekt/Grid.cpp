#include "stdafx.h"
#include "Grid.h"

Grid::Grid(Object* object)
	: object(object), rows(8), nodeSize(4.0f), objectOffset(5)
{
	
}

Grid::~Grid()
{
}

void Grid::Init()
{
}

bool Grid::CheckInGrid()
{
	dx::XMFLOAT3 objectPos;
	dx::XMStoreFloat3(&objectPos, object->GetTransform().GetPosition());

	int gridPosX = std::round((objectPos.x - objectOffset) / nodeSize);
	int gridPosZ = std::round((objectPos.z - objectOffset) / nodeSize);
	//int gridPosZ = (std::round((objectPos.z - objectOffset) / nodeSize) * rows) + 1;
	int gridPos = gridPosX * gridPosZ;
	dx::XMFLOAT2 startNode = dx::XMFLOAT2(gridPosX, gridPosZ);
	dx::XMFLOAT2 endNode = dx::XMFLOAT2(8, 0);
	std::vector<aStar*> nodeValue;

	for (int x = 0; x < rows; x++)
	{
		for (int z = 0; z < rows; z++)
		{
			nodeValue[x+z]->nextNode = 
			if (gridPosX == x && gridPosZ == z)
			{
				box.DrawBox(dx::XMFLOAT3((x * nodeSize) + 5, 4, (z * nodeSize) + 5), dx::XMFLOAT3(2, 2, 2), dx::XMFLOAT3(0, 1, 0));
			}	
			else
				box.DrawBox(dx::XMFLOAT3((x * nodeSize) + 5, 4, (z * nodeSize) + 5), dx::XMFLOAT3(2, 2, 2), dx::XMFLOAT3(1, 0, 0));
		}
	}

	
	if (gridPos == 0)
	{
		//grids[gridPos];
		//std::cout << "You are in a grid" << std::endl;
	}
	//std::cout << "Grid pos: " << gridPos << std::endl;
	//std::cout << "x: " << objectPos.x << ", z: " << objectPos.z << std::endl;

	//std::cout << test << std::endl;

	return false;
}

void Grid::PriorityQueue(int cost)
{
	maxHeap.push(cost);
}
