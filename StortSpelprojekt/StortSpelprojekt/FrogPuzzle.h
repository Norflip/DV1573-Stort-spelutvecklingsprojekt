#pragma once
#include "ResourceManager.h"

class FrogPuzzle
{
public:
	FrogPuzzle(ResourceManager* resourceManager);
	virtual ~FrogPuzzle();
	void Initialize();
	void Update(const float& deltaTime);
	void AddPuzzleElements(Object* frog, Object* fly);

private:
	ResourceManager* resource;
	Object* frog;
	Object* fly;
};