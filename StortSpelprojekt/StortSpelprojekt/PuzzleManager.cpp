#include "stdafx.h"
#include "PuzzleManager.h"

PuzzleManager::PuzzleManager(ResourceManager* resourceManager, Object* player, Object* house)
{
	this->resourceManager = resourceManager;
	this->player = player;
	this->house = house;		
}

PuzzleManager::~PuzzleManager()
{
}
