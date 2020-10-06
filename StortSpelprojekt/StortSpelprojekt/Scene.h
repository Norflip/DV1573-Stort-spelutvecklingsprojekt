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
#include "GUIManager.h"
#include "Skybox.h"
#include "WorldGenerator.h"
#include "SaveState.h"
#include <string>

class GUIFont;
class SpriteRenderPass;
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
	dx::SpriteBatch* spriteBatch;
	WorldGenerator worldGenerator;
	SpriteRenderPass* spritePass;
	/* test sky */
	Object* testSkybox;
	GUIManager* guiManager;
};