#pragma once


//THIS MESS NEEDS TO DIE A HORRIBLE DEATH
#include "Object.h"
#include "HeightMap.h"
#include "MoveComponent.h"
#include "ControllerComp.h"
#include "SkeletonMeshComponent.h"
#include "PointLightComponent.h"
#include "NodeWalkerComp.h"
#include "PlayerAttackComp.h"
#include "SaveState.h"
#include "GUIManager.h"
#include "Physics.h"
#include "RigidBodyComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "DebugBoxShapeComponent.h"
#include "PickupComponent.h"
#include "ParticlesystemComponent.h"
//Might need to yeet
#include "ResourceManager.h"
#include "ObjectPooler.h"
#include "WeaponComponent.h"
#include "EnemyManager.h"
#include "Grid.h"
#include "PlayerAnimHandlerComp.h"

class GUIFont;
class SpriteRenderPass;
class PlayerComp;

constexpr auto IMMORTAL = 1;

ALIGN16
class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void SetDepedencies(ResourceManager* manager, Renderer* renderer, Physics* physics, Window* window);

	virtual void Initialize() = 0;
	virtual void InitializeObjects() = 0;
	virtual void InitializeGUI() = 0;
	
	virtual void OnActivate() = 0;
	virtual void OnDeactivate() = 0;
	
	virtual void Update(const float& deltaTime);
	virtual void FixedUpdate(const float& fixedDeltaTime);
	virtual void Render();
	
	// Add object to the scene hierarchy
	void AddObject(Object* object);

	// Add object as a child to another object
	void AddObject(Object* object, Object* parent);

	void RemoveObject(Object* object);
	Object* GetRoot() const { return this->root; }

	void PrintSceneHierarchy(Object* object, size_t level) const;
	
protected:
	Object* root;
	Renderer* renderer;
	ResourceManager* resources;
	Physics* physics;
	Window* window;
	
	std::queue<Object*> removeQueue;
	CameraComponent* camera;
	
	GameClock clock;

	Input& input;
	Object* enemy;
	Object* player;
	EnemyManager* enemyManager;
	Grid* grid;

	ObjectPooler* pooler;
	GUIManager* guiManager;		
};