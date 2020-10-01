#pragma once
#include <vector>
#include "Object.h"
#include "HeightMap.h"
#include "ShittyOBJLoader.h"
#include "Input.h"
#include "GameClock.h"
#include "CameraComponent.h"
#include "MoveComponent.h"
#include "ControllerComponent.h"
#include "MeshComponent.h"
#include "ZWEBLoader.h"
#include "SkeletonMeshComponent.h"
#include "NodeWalkerComponent.h"

#include "Skybox.h"
#include "WorldGenerator.h"
#include "SaveState.h"

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

	/* new - render scene to texture */
	void RenderSceneToTexture();

	void AddObject(Object* object);
	void AddObject(Object* object, Object* parent);

	void RemoveObject(Object* object);
	Object* GetRoot() const { return this->root; }

	void PrintSceneHierarchy(Object* object, size_t level) const;

	
private:	
	Object* root;
	CameraComponent* camera;
	Renderer* renderer;

	Input& input;

	/* Test stuff */
	Texture* screenquadTex;
	Object* quad;


	/* Test skybox in class */
	Object* skybox;
	Skybox* skyboxClass;		

	WorldGenerator worldGenerator;	
};