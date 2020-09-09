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
	
	void OnActivate() {}
	void OnDeactivate() {}
	
	void ProcessInput();
	void Update(const float& deltaTime);
	void FixedUpdate(const float& fixedDeltaTime);
	void Render();

private:	
	std::vector<Object> objects;
	Camera camera;
	DXHandler* dxHandler;
	Renderer* renderer;
};