#pragma once
#include <vector>
#include "Object.h"
#include "ShittyOBJLoader.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void Initialize(DXHandler* dxHandler, Renderer* renderer);
	void Update(const float& deltaTime);
	void Render();

private:	
	std::vector<Object> objects;
	Camera camera;
	DXHandler* dxHandler;
	Renderer* renderer;
};