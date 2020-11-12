#include "stdafx.h"
#include "GridMap.h"

GridMap::GridMap(Object* object)
	: object(object)
{
	Grid temp;
	temp.buttomLeft = dx::XMFLOAT2(0, 0);
	temp.buttomRight = dx::XMFLOAT2(0, 10);
	temp.topRight = dx::XMFLOAT2(10, 10);
	temp.topLeft = dx::XMFLOAT2(10, 0);
	temp.position = dx::XMFLOAT2(0, 0);
	grids.push_back(temp);
	oneGrid = temp;
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

	int gridWidth = 10;
	int gridHeight = 10;
	int gridPosX = std::round(objectPos.x / 10.0f) * 10.0f;
	int gridPosY = std::round(objectPos.z / 10.0f) * 10.0f;

	int gridPos = gridPosX + gridPosY;
	if (gridPos == 0)
	{
		//grids[gridPos];
		//std::cout << "You are in a grid" << std::endl;
	}
	std::cout << "Grid pos: " << gridPosX << std::endl;
	std::cout << "x: " << objectPos.x << ", z: " << objectPos.z << std::endl;

	//std::cout << test << std::endl;

	return false;
}
