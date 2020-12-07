#pragma once

//THIS MESS NEEDS TO DIE A HORRIBLE DEATH
#include "Object.h"
#include "HeightMap.h"
#include "MoveComponent.h"
#include "ControllerComp.h"
#include "SkeletonMeshComponent.h"
#include "LightComponent.h"
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
#include "SelectableComponent.h"
#include "ParticlesystemComponent.h"
//Might need to yeet
#include "ResourceManager.h"
#include "ObjectPooler.h"
#include "WeaponComponent.h"
#include "EnemyManager.h"
#include "Pathfinding.h"
#include <wchar.h>
#include "PlayerAnimHandlerComp.h"
#include "HousePartsComponent.h"

#include "ParticleComponent.h"

#include "Particlesys.h"


class GUIFont;
class SpriteRenderPass;
class PlayerComp;

ALIGN16
class Scene
{
public:
	Scene(const std::string& debugName = "[DEFAULT_SCENE]");
	virtual ~Scene();
	virtual void OnResize(UINT width, UINT height);
	virtual void SetDepedencies(ResourceManager* manager, Renderer* renderer, Physics* physics, Window* window);

	virtual void Initialize() = 0;
	virtual void InitializeObjects() = 0;
	virtual void InitializeGUI() = 0;
	
	virtual void OnActivate() = 0;
	virtual void OnDeactivate() = 0;
	
	virtual void Update(const float& deltaTime);
	virtual void FixedUpdate(const float& fixedDeltaTime);
	virtual void Render();
	
#if USE_IMGUI
	virtual void OnIMGUIFrame() {};
#endif
	// Add object to the scene hierarchy
	void AddObjectToRoot(Object* object);
	Object* GetRoot() const { return this->root; }

	void PrintSceneHierarchy(Object* object, size_t level) const;
	virtual void AnimateIcon();

private:
	void DebugCheckForObjectIDConflict(Object* current, std::unordered_set<size_t>& registry, size_t& conflicCounter) const;

protected:
	int amountOfFrames = 8;
	int currentframe = 0;
	int frameToDraw = 1;
	int lastFrame = 1;
	float physicsDelay = 0.f;
	float loadScreenDelay = 2.0f;
	float delayTimer = 0;
	wchar_t iconText[10];

	Object* root;
	Renderer* renderer;
	ResourceManager* resources;
	Physics* physics;
	Window* window;
	
	std::queue<Object*> removeQueue;
	CameraComponent* camera;
	bool firstFrame;
	bool sceneSwitch = false;
	GameClock clock;

	Input& input;
	Object* player;
	EnemyManager* enemyManager;

	ObjectPooler* pooler;
	GUIManager* guiManager;		
	std::string debugName;

};