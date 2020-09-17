#pragma once
#include <vector>
#include "Object.h"
#include "HeightMap.h"
#include "ShittyOBJLoader.h"
#include "Input.h"
#include "CameraComponent.h"
#include "MeshComponent.h"

class Scene
{
public:
	Scene();
	virtual ~Scene();

	void Initialize(Renderer* renderer);
	
	void OnActivate() {}
	void OnDeactivate() {}
	
	void Update(const float& deltaTime);
	void FixedUpdate(const float& fixedDeltaTime);
	void Render();

	void PrintSceneHierarchy() const;
	void PrintSceneHierarchy(Object* object, size_t level) const;

private:	
	std::vector<Object*> objects;
	CameraComponent* camera;
	Renderer* renderer;
};