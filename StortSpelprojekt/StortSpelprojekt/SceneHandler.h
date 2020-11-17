#pragma once
#include "ChunkGrid.h"

class SceneHandler
{
public:
	SceneHandler();
	virtual~SceneHandler();

	void CreateWorld();
	void DestroyWorld();
private:
	ChunkGrid cGrid;
};
