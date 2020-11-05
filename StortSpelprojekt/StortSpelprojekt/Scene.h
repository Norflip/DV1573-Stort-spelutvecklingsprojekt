#pragma once

enum NEXT_SCENE { INTRO = 0, LOSE = 1, GAME = 2, WIN = 3 };

//THIS MESS NEEDS TO DIE A HORRIBLE DEATH
#include "Object.h"
#include "HeightMap.h"
#include "MoveComponent.h"
#include "ControllerComp.h"
#include "SkeletonMeshComponent.h"
#include "PointLightComponent.h"
#include "NodeWalkerComp.h"
#include "EnemyStatsComp.h"
#include "EnemySMComp.h"
#include "PlayerAttackComp.h"
#include "Skybox.h"
#include "SaveState.h"
#include "GUIManager.h"
#include "Physics.h"
#include "RigidBodyComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "DebugBoxShapeComponent.h"
#include "PickupComponent.h"
//Might need to yeet
#include "ResourceManager.h"
#include "ObjectPooler.h"
#include "WeaponComponent.h"

class GUIFont;
class SpriteRenderPass;
class PlayerComp;

constexpr auto immortal = 1;

ALIGN16
class Scene
{
public:
	Scene();
	virtual ~Scene();

	virtual void SetDepedencies(ResourceManager* manager, Renderer* renderer, Window* window);

	virtual void Initialize() = 0;
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

	bool Quit();
	NEXT_SCENE nextScene;
	
private:

protected:
	Object* root;
	Renderer* renderer;
	ResourceManager* resources;
	Window* window;

	CameraComponent* camera;
	
	GameClock clock;

	Input& input;
	Object* enemy;
	Object* player;

	//EnemyStatsComp* enemyStatsComp;
	//PlayerComp* playerStatsComp;
	dx::SpriteBatch* spriteBatch;
	SpriteRenderPass* spritePass;	

	ObjectPooler* pooler;
	GUIManager* guiManager;		
	
	Skybox* skyboxClass;		

	bool quit;	

};