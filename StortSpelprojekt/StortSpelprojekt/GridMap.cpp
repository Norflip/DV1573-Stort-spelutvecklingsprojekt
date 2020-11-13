#include "stdafx.h"
#include "GridMap.h"

GridMap::GridMap(Object* object)
	: object(object), rows(5), gridOffset(65), gridSize(10.0f), objectOffset(5)
{
	Grid temp;
	grids.push_back(temp);
}

GridMap::~GridMap()
{
}

void GridMap::Init()
{

}

bool GridMap::CheckInGrid()
{
	dx::XMFLOAT3 objectPos;
	dx::XMStoreFloat3(&objectPos, object->GetTransform().GetPosition());

	int gridPosX = std::round((objectPos.x - objectOffset) / gridSize);
	int gridPosZ = (std::round((objectPos.z - objectOffset) / gridSize) * rows) + 1;
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
