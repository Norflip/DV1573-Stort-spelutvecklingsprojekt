#pragma once
#include "PlayerComp.h"
#include "ResourceManager.h"
#include "FrogPuzzle.h"

class PuzzleManager
{
private:
	Object* player;
	Object* house;
	ResourceManager* resourceManager;
	FrogPuzzle* frogPuzzle;

public:
	PuzzleManager(ResourceManager* resourceManager, Object* player, Object* house);
	virtual ~PuzzleManager();
	void InitializePuzzles();
	void Update(const float& deltaTime);
	
};