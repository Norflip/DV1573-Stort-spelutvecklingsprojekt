#include "stdafx.h"
#include "Grid.h"

Grid::Grid(Object* object)
	: object(object), rows(4), nodeSize(8.0f), objectOffset(5)
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
	for (int x = 0; x < rows; x++)
	{
		for (int z = 0; z < rows; z++)
		{
			box.DrawBox(dx::XMFLOAT3(x * nodeSize, 4, z * nodeSize), dx::XMFLOAT3(4, 2, 4), dx::XMFLOAT3(1, 0, 1));
		}
	}
	dx::XMFLOAT3 objectPos;
	dx::XMStoreFloat3(&objectPos, object->GetTransform().GetPosition());

	int gridPosX = std::round((objectPos.x - objectOffset) / nodeSize);
	int gridPosZ = (std::round((objectPos.z - objectOffset) / nodeSize) * rows) + 1;
	int gridPos = gridPosX * gridPosZ;
	
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
