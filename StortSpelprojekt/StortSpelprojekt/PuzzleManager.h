#pragma once
#include "PlayerComp.h"

class PuzzleManager
{
private:
	Object* player;


public:
	PuzzleManager();
	virtual ~PuzzleManager();
	void Update(const float& deltaTime);

};