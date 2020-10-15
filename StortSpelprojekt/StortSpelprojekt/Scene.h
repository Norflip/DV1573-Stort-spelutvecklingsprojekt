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
#include "PointLightComponent.h"
#include "NodeWalkerComponent.h"

#include "Skybox.h"
#include "WorldGenerator.h"
#include "SaveState.h"
#include "DShape.h"
#include "GUIManager.h"
#include <string>

#include "Physics.h"
#include "RigidBodyComponent.h"
#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "DebugBoxShapeComponent.h"

#include "ResourceManager.h"
class GUIFont;
class SpriteRenderPass;

class Scene
{
public:
	Scene(ResourceManager* manager);
	virtual ~Scene();

	virtual void Initialize(Renderer* renderer) = 0;
	virtual void InitializeObjects() = 0;
	virtual void InitializeGUI() = 0;

	// What is this for?
	virtual void OnActivate() = 0;
	virtual void OnDeactivate() = 0;
	
	virtual void Update(const float& deltaTime);
	virtual void FixedUpdate(const float& fixedDeltaTime);
	virtual void Render();
	
	void AddObject(Object* object);
	void AddObject(Object* object, Object* parent);

	void RemoveObject(Object* object);
	Object* GetRoot() const { return this->root; }

	void PrintSceneHierarchy(Object* object, size_t level) const;

	
private:

protected:
	Object* root;
	CameraComponent* camera;
	Renderer* renderer;
	GameClock clock;
	Input& input;

	dx::SpriteBatch* spriteBatch;
	WorldGenerator worldGenerator;
	SpriteRenderPass* spritePass;	

	GUIManager* guiManager;		
	
	Skybox* skyboxClass;		

	ResourceManager* resourceManager;
};