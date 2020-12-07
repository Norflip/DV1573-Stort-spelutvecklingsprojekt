#include "stdafx.h"
#include "GameScene.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"
#include "GUICompass.h"
#include "SaveHandler.h"
#include "Config.h"

#if USE_IMGUI
#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_win32.h"
#include "Imgui\imgui_impl_dx11.h"
#endif

bool GameScene::immortal = false;
bool GameScene::drawColliders = false;
#include <d3d11_4.h>
#include <dxgi1_6.h>
#include <psapi.h>

GameScene::GameScene() : Scene("GameScene")
{
	this->interiorPosition = { 0.0f, -100.0f, 0.0f };
	fogCol = 0;
	end = false;
	firstFrame = false;
}

GameScene::~GameScene()
{
}

void GameScene::Initialize()
{
	InitializeGUI();
	InitializeObjects();
	InitializeInterior();
}

void GameScene::InitializeObjects()
{
	Object* houseBaseObject = new Object("houseBase");
	Object* housesLegsObject = new Object("houseLegs");
	Object* houseDoor = resources->AssembleObject("HouseDoor", "HouseDoorMaterial");
	Object* houseExterior = resources->AssembleObject("HouseExterior", "HouseExteriorMaterial");
	houseExterior->GetComponent<MeshComponent>()->GetMaterials()[0]->SetTransparent(true);

	houseBaseObject->GetTransform().Rotate(0, -90.0f * Math::ToRadians, 0.0);

	house = houseBaseObject;

	SkeletonMeshComponent* baseComponent = resources->GetResource<SkeletonMeshComponent>("HouseSkeleton");
	SkeletonMeshComponent* legsComponent = resources->GetResource<SkeletonMeshComponent>("HouseLegsSkeleton");

	houseBaseObject->GetTransform().SetScale({ 0.5f, 0.5f, 0.5f });

	//WALLS
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(2.0f, 3.5f, 2.3f), dx::XMFLOAT3(0.f, 0.9f, -1.0f));
	//PORCH
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(3.0f, 0.5f, 3.0f), dx::XMFLOAT3(0.f, 0.2f, 0.f));
	//FENCE BACK
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.125f, 0.625f, 3.375f), dx::XMFLOAT3(-3.3f, 2.f, 0.05f));
	//FENCE FRONT
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.1f, 3.5f, 1.8f), dx::XMFLOAT3(3.5f, 0.7f, 0.5f));
	//FENCE RIGHT
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(3.f, 0.625f, 0.1f), dx::XMFLOAT3(0.f, 3.f, -3.f));
	//FENCE LEFT
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(3.3f, 0.625f, 0.15f), dx::XMFLOAT3(0.f, 3.f, 3.f));
	// RB
	houseBaseObject->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);

	baseComponent->SetTimeScale(1.0f);
	legsComponent->SetTimeScale(1.0f);
	baseComponent->SetTrack(SkeletonStateMachine::IDLE, false);
	legsComponent->SetTrack(SkeletonStateMachine::IDLE, false);

	houseBaseObject->AddComponent<SkeletonMeshComponent>(baseComponent);
	housesLegsObject->AddComponent<SkeletonMeshComponent>(legsComponent);

	Object::AddToHierarchy(houseBaseObject, housesLegsObject);
	Object::AddToHierarchy(houseBaseObject, houseDoor);
	Object::AddToHierarchy(houseBaseObject, houseExterior);


	nodeWalker = houseBaseObject->AddComponent<NodeWalkerComp>();
	nodeWalker->InitAnimation();
	AddObjectToRoot(houseBaseObject);

	Object* rigidDoor = new Object("door_body", ObjectFlag::ENABLED);
	Object::AddToHierarchy(houseDoor, rigidDoor);
	rigidDoor->GetTransform().SetPosition({ 2, 2, 2 });

	houseExterior->AddComponent<HousePartsComponent>(houseBaseObject->GetComponent<SkeletonMeshComponent>());

	//houseDoor->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.3f, 0.85f, 0.3f), dx::XMFLOAT3(0,0,0));
	//houseDoor->AddComponent<SphereColliderComponent>(1.0f, dx::XMFLOAT3(3.533f, 2.884f, 4.02f));
	
//	rigidDoor->AddComponent<SphereColliderComponent>(2.0f, dx::XMFLOAT3(0,0,0));

	rigidDoor->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.3f, 0.85f, 0.3f), dx::XMFLOAT3(0, 0, 0));
	rigidDoor->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::DOOR, FilterGroups::EVERYTHING | ~FilterGroups::PLAYER, BodyType::STATIC, true);

	houseDoor->AddComponent<HousePartsComponent>(houseBaseObject->GetComponent<SkeletonMeshComponent>());


	//AddObjectToRoot(houseDoor);
	//AddObjectToRoot(houseExterior);

	//Player & Camera
	dx::XMFLOAT3 playerSpawn = { 10,2,10 };
	dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
	dx::XMVECTOR playerSpawnVec = dx::XMLoadFloat3(&playerSpawn);
	Object* playerObject = new Object("player", ObjectFlag::ENABLED);
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	this->player = playerObject;
	camera = cameraObject->AddComponent<CameraComponent>(window->GetWidth(), window->GetHeight(), Config::GetInt("FOV", 70));


	cameraObject->GetTransform().SetPosition(playerSpawnVec);
	playerObject->GetTransform().SetPosition(playerSpawnVec);
	playerObject->AddComponent<CapsuleColliderComponent>(0.4f, 1.9f, zero);
	playerObject->AddComponent<RigidBodyComponent>(50.f, FilterGroups::PLAYER, (FilterGroups::EVERYTHING), BodyType::DYNAMIC, true);

	playerObject->AddComponent<PlayerComp>(renderer, camera, house, Engine::Instance->GetPhysics(), guiManager, 100.f, 2.f, 40.f, 50.f, 3.f);
	playerObject->AddComponent<ControllerComp>(cameraObject, houseBaseObject);
	playerObject->GetComponent<PlayerComp>()->SetInteriorPosition(this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z);

	Object::AddToHierarchy(playerObject, cameraObject);
	AddObjectToRoot(playerObject);

	// Light around player
	Object* playerLight = new Object("playerLight");
	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(0.0f, 0.f, 0.0f);
	playerLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	LightComponent* playerLightComponent = playerLight->AddComponent<LightComponent>(0, dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), 4.f);
	playerLightComponent->SetEnabled(true);
	playerLightComponent->SetIntensity(1.0f);
	Object::AddToHierarchy(playerObject, playerLight);

	// Sunlight
	Object* sunLight = new Object("sunLight");
	dx::XMFLOAT3 sunTranslation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	sunLight->GetTransform().SetPosition(dx::XMLoadFloat3(&sunTranslation));
	LightComponent* sunComponent = sunLight->AddComponent<LightComponent>(2, dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), 7.f);
	sunComponent->SetEnabled(true);
	sunComponent->SetIntensity(0.2f);
	dx::XMFLOAT3 sunDirection;
	dx::XMStoreFloat3(&sunDirection, dx::XMVector3Normalize(dx::XMVectorSet(0, -1, 1, 0)));
	sunComponent->SetDirection(sunDirection);
	AddObjectToRoot(sunLight);

	/* For fuel info from playercomp */
	nodeWalker->GetPlayerInfo(playerObject->GetComponent<PlayerComp>());

	world.Initialize(root, resources, renderer);

	//Player Arms
	Object* playerArms = new Object("PlayerArms", ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);
	SkeletonMeshComponent* armsSkeleton = resources->GetResource<SkeletonMeshComponent>("PlayerArmsSkeleton");
	playerArms->AddComponent<SkeletonMeshComponent>(armsSkeleton);
	playerArms->AddComponent<PlayerAnimHandlerComp>(playerArms->GetComponent<SkeletonMeshComponent>(), cameraObject, player);
	player->GetComponent< PlayerComp>()->InsertArms(playerArms);
	AddObjectToRoot(playerArms);

	//Axe
	Object* axeObject = resources->AssembleObject("Axe", "AxeMaterial", false, ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);
	axeObject->GetTransform().SetPosition({ 21.0f, 1.0f, -16.0f });
	axeObject->GetTransform().SetScale({ 1.0f, 1.0f, 1.0f });
	axeObject->AddComponent<WeaponComponent>(playerArms->GetComponent<SkeletonMeshComponent>());
	playerObject->GetComponent<PlayerComp>()->InsertWeapon(axeObject->GetComponent<WeaponComponent>(), axeObject->GetName());
	AddObjectToRoot(axeObject);

	roadSign = resources->AssembleObject("Endsign", "EndsignMaterial");
	roadSign->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 2.0f, 3.0f, 4.0f }, dx::XMFLOAT3{ 0,0,0 });
	roadSign->AddComponent<SelectableComponent>();
	roadSign->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::CLICKABLE, (FilterGroups::EVERYTHING & ~FilterGroups::PLAYER), BodyType::STATIC, true);
	roadSign->RemoveFlag(ObjectFlag::ENABLED);
	AddObjectToRoot(roadSign);

	rightSign = resources->AssembleObject("LeftDirectionSign", "LeftDirectionSignMaterial");
	leftSign = resources->AssembleObject("RightDirectionSign", "RightDirectionSignMaterial");

	rightSign->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 1.0f, 1.0f, 1.0f }, dx::XMFLOAT3{ 0, 0, 0 });
	rightSign->AddComponent<SelectableComponent>();
	rightSign->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::CLICKABLE, (FilterGroups::EVERYTHING & ~FilterGroups::PLAYER), BodyType::STATIC, true);

	leftSign->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 1.0f, 1.0f, 1.0f }, dx::XMFLOAT3{ 0, 0, 0 });
	leftSign->AddComponent<SelectableComponent>();
	leftSign->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::CLICKABLE, (FilterGroups::EVERYTHING & ~FilterGroups::PLAYER), BodyType::STATIC, true);

	AddObjectToRoot(rightSign);
	AddObjectToRoot(leftSign);

	//LOADING BASE MONSTER; ADDING SKELETONS TO IT
	enemyManager = new EnemyManager();
	enemyManager->Initialize(player, house, player->GetComponent<PlayerComp>(), camera, root);
	enemyManager->InitBaseEnemy();
	enemyManager->InitChargerEnemy();

	GUICompass* compass = new GUICompass(*renderer, window, house, player);
	guiManager->AddGUIObject(compass, "compass");
}

void GameScene::InitializeGUI()
{
	float windowWidth = FCAST(window->GetWidth());
	float windowHeight = FCAST(window->GetHeight());

	//INFO, WE ARE DRAWING BACK TO FRONT. IF YOU WANT SOMETHING TO BE IN FRONT. SET VALUE TO 0. IF YOU WANT IT IN BACK USE 0.1 -> 1

	//BUTTONS AT LEFT SIDE
	GUISprite* equimpmentSprite1 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 10, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* equimpmentSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 90, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* equimpmentSprite3 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 170, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* equimpmentSprite4 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 250, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);

	//BARS THAT SCALE
	GUISprite* fuelScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 170, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 90, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 10, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);

	//BARS AT RIGHT SIDE
	GUISprite* fuelBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 170, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 90, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 10, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);

	//ICONS ON TOP OF ITEMS
	GUISprite* equimpmentSpriteAxe = new GUISprite(*renderer, "Textures/AxeIcon2.png", 10, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);

	//ICONS ON TOP OF BARS
	GUISprite* fuelSprite = new GUISprite(*renderer, "Textures/FireIcon.png", 170, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodSprite = new GUISprite(*renderer, "Textures/FoodIcon.png", 90, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthSprite = new GUISprite(*renderer, "Textures/HealthIcon.png", 10, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);

	// Info gui on the door
	GUISprite* infoSprite = new GUISprite(*renderer, "Textures/Info.png", 0, 0, 0.0f, DrawDirection::Default, ClickFunction::NotClickable);
	// Loading
	GUISprite* loadSprite = new GUISprite(*renderer, "Textures/Loading.png", 0, 0, -110, DrawDirection::Default, ClickFunction::NotClickable, GuiGroup::Load);
	loadSprite->SetVisible(true);
	//FONTS
	//GUIFont* playerPosDisplay = new GUIFont(*renderer, "playerPos", 50, 100);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", 30, 30);
	//GUIFont* healthDisplay = new GUIFont(*renderer, "playerHealth", 50, 100);
	//GUIFont* enemyDisplay = new GUIFont(*renderer, "enemyHealth", 50, 150);

	//CROSSHAIR
	GUISprite* dot = new GUISprite(*renderer, "Textures/TestFix2.png", (windowWidth / 2) - 6, (windowHeight / 2) - 6, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* crosshair = new GUISprite(*renderer, "Textures/Crosshair.png", (windowWidth / 2) - 25, (windowHeight / 2) - 25, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* doorSprite = new GUISprite(*renderer, "Textures/DoorSprite.png", (windowWidth / 2) - 6, (windowHeight / 2) - 6, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* fuel = new GUISprite(*renderer, "Textures/Fuel_Icon.png", (windowWidth / 2) - 6, (windowHeight / 2) - 6, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);

	GUIFont* vramDisplay = new GUIFont(*renderer, "vram", 30, 60);
	GUIFont* ramDisplay = new GUIFont(*renderer, "ram", 30, 90);

	crosshair->SetVisible(false);
	doorSprite->SetVisible(false);
	fuel->SetVisible(false);
	infoSprite->SetVisible(false);

	// INSERTIONS
	guiManager = new GUIManager(renderer, 0);
	guiManager->AddGUIObject(fpsDisplay, "fps");
	//guiManager->AddGUIObject(playerPosDisplay, "playerPos");
	//guiManager->AddGUIObject(healthDisplay, "playerHealth");
	//guiManager->AddGUIObject(enemyDisplay, "enemyHealth");
	//COMPASS


	// Loading
	guiManager->AddGUIObject(loadSprite, "loading");
	//BASE OF EQUIPMENT
	guiManager->AddGUIObject(equimpmentSprite1, "equimpmentSprite1");
	guiManager->AddGUIObject(equimpmentSprite2, "equimpmentSprite2");
	guiManager->AddGUIObject(equimpmentSprite3, "equimpmentSprite3");
	guiManager->AddGUIObject(equimpmentSprite4, "equimpmentSprite4");

	//BASE OF DIPPING BARS
	foodScalingBar->SetScale(1.0f, 0.0f);
	healthScalingBar->SetScale(1.0f, 0.0f);
	fuelScalingBar->SetScale(1.0f, 0.0f);
	guiManager->AddGUIObject(foodScalingBar, "foodDippingBar");
	guiManager->AddGUIObject(healthScalingBar, "healthDippingBar");
	guiManager->AddGUIObject(fuelScalingBar, "fuelDippingBar");

	//ICON OF EQUIPMENT
	guiManager->AddGUIObject(equimpmentSpriteAxe, "equimpmentSpriteAxe");

	//BASE OF BARS
	guiManager->AddGUIObject(fuelBar, "fuelBar");
	guiManager->AddGUIObject(foodBar, "foodBar");
	guiManager->AddGUIObject(healthBar, "healthBar");

	//ICON OF BARS
	guiManager->AddGUIObject(fuelSprite, "fuelSprite");
	guiManager->AddGUIObject(foodSprite, "foodSprite");
	guiManager->AddGUIObject(healthSprite, "healthSprite");

	// Info gui on the door
	guiManager->AddGUIObject(infoSprite, "infoSprite");

	//CROSSHAIR
	guiManager->AddGUIObject(dot, "dot");
	guiManager->AddGUIObject(crosshair, "crosshair");
	guiManager->AddGUIObject(doorSprite, "door");
	guiManager->AddGUIObject(fuel, "fuel");

	guiManager->AddGUIObject(vramDisplay, "vramSprite");
	guiManager->AddGUIObject(ramDisplay, "ramSprite");
}

void GameScene::InitializeInterior()
{

	// Inside house
	Object* houseInterior = resources->AssembleObject("HouseInterior", "HouseInteriorMaterial");
	houseInterior->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z, 0 });
	houseInterior->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(10.0f, 0.5f, 10.0f), dx::XMFLOAT3(0, 0, 0)); // Floor
	houseInterior->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.5f, 10.0f, 10.0f), dx::XMFLOAT3(5.0f, 0, 0)); // Right wall
	houseInterior->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.5f, 10.0f, 10.0f), dx::XMFLOAT3(-9.0f, 0, 0)); // Left wall
	houseInterior->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(10.0f, 10.0f, 1.5f), dx::XMFLOAT3(0, 0, 7.0f)); // Front wall
	houseInterior->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(10.0f, 10.0f, 1.5f), dx::XMFLOAT3(0, 0, -8.0f)); // Back wall
	houseInterior->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true); // RB
	houseInterior->GetComponent<RigidBodyComponent>()->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
	AddObjectToRoot(houseInterior);

	Object* fireplace = resources->AssembleObject("Fireplace", "FireplaceMaterial");
	fireplace->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	fireplace->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.5f, 5.0f, 1.0f), dx::XMFLOAT3(-8.1f, 0, -1.3f));
	fireplace->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(fireplace);

	Object* logs = resources->AssembleObject("Logs", "LogsMaterial");
	logs->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	logs->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.0f, 1.0f, 1.0f), dx::XMFLOAT3(-8.1f, 1.0f, -1.3f));
	logs->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::FIRE, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(logs);

	Object* curtains = resources->AssembleObject("Curtains", "CurtainsMaterial");
	Object::AddToHierarchy(houseInterior, curtains);

	Object* bed = resources->AssembleObject("Bed", "BedMaterial");
	bed->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	bed->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(2.5f, 1.0f, 2.0f), dx::XMFLOAT3(-2.0f, 1.0f, 5.f));
	bed->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(bed);

	Object* bookShelf = resources->AssembleObject("BookShelf", "BookShelfMaterial");
	bookShelf->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	bookShelf->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.0f, 5.0f, 1.5f), dx::XMFLOAT3(1.0f, 1.0f, -7.f));
	bookShelf->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(bookShelf);

	Object* chair = resources->AssembleObject("Chair", "ChairMaterial");
	chair->GetTransform().SetPosition({ this->interiorPosition.x - 2.0f, this->interiorPosition.y, this->interiorPosition.z + 2.0f });
	chair->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 2.0f, 1.0f), dx::XMFLOAT3(-4.0f, 1.0f, 1.5f));
	chair->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(chair);

	Object* sink = resources->AssembleObject("Sink", "SinkMaterial");
	sink->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	sink->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 2.0f, 1.5f), dx::XMFLOAT3(-1.5f, 1.0f, -7.f));
	sink->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(sink);

	Object* stove = resources->AssembleObject("Stove", "StoveMaterial");
	stove->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	stove->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 2.0f, 1.5f), dx::XMFLOAT3(-2.5f, 1.0f, -7.f));
	stove->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(stove);

	Object* insideDoor = resources->AssembleObject("InsideDoor", "InsideDoorMaterial");
	insideDoor->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	insideDoor->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.2f, 2.0f, 1.5f), dx::XMFLOAT3(3.0f, 2.0f, -3.0f));
	insideDoor->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::DOOR, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(insideDoor);

	Object* table = resources->AssembleObject("Table", "TableMaterial");
	table->GetTransform().SetPosition({ this->interiorPosition.x, this->interiorPosition.y, this->interiorPosition.z });
	table->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(2.0f, 1.4f, 2.0f), dx::XMFLOAT3(-6.5f, 0.0f, -5.8f));
	table->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);
	AddObjectToRoot(table);

	Object* tutorialFood = resources->AssembleObject("Fruits", "FruitsMaterial", true);
	tutorialFood->GetTransform().SetPosition({ -5.65f, interiorPosition.y + 1.0f, -4.6f, 0.0f });
	tutorialFood->AddComponent<PickupComponent>(PickupType::Food, 30.0f);
	tutorialFood->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.25f, 0.25f, 0.25f), dx::XMFLOAT3(0, 0, 0));
	tutorialFood->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::DYNAMIC, true);
	AddObjectToRoot(tutorialFood);

	Object* tutorialHealth = resources->AssembleObject("HealthKit", "HealthKitMaterial", true);
	tutorialHealth->GetTransform().SetPosition({ -5.0f, interiorPosition.y + 1.0f, -4.4f, 0.0f });
	tutorialHealth->AddComponent<PickupComponent>(PickupType::Health, 30.0f);
	tutorialHealth->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.25f, 0.1f, 0.25f), dx::XMFLOAT3(0, 0, 0));
	tutorialHealth->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::PICKUPS, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::DYNAMIC, true);
	AddObjectToRoot(tutorialHealth);

	Object* tutorialFuel = resources->AssembleObject("BlueFuel", "BlueFuelMaterial", false);
	tutorialFuel->GetTransform().SetPosition({ -5.0f, interiorPosition.y + 3.0f, 0.11f, 0.0f });
	tutorialFuel->AddComponent<PickupComponent>(PickupType::Fuel, 30.0f);
	tutorialFuel->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.3f, 0.35f, 0.15f), dx::XMFLOAT3(0, 0, 0));
	tutorialFuel->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::HOLDABLE, FilterGroups::EVERYTHING & ~FilterGroups::PLAYER, BodyType::DYNAMIC, true);
	AddObjectToRoot(tutorialFuel);

	Object* fireLight = new Object("fireLight");
	LightComponent* fLight = fireLight->AddComponent<LightComponent>(LightType::POINT_LIGHT, dx::XMFLOAT4(1.0f, 0.29f, 0.0f, 1.0f), 2.2f);
	fireLight->GetTransform().SetPosition({ -7.0f, -99.f, -1.36f });
	//fireLight->AddComponent<ParticleSystemComponent>(renderer, Engine::Instance->GetResources()->GetShaderResource("particleShader"));
	//fireLight->GetComponent<ParticleSystemComponent>()->InitializeFirelikeParticles(renderer->GetDevice(), L"Textures/fire1.png");
	fireLight->AddFlag(ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);
	fLight->SetEnabled(true);
	fLight->SetIntensity(1.f);
	AddObjectToRoot(fireLight);

	testParticles = new Particlesys(/*Engine::Instance->GetResources()->GetShaderResource("soshader"), Engine::Instance->GetResources()->GetShaderResource("drawshader")*/);
	testParticles->InitializeParticleShaders(renderer->GetDevice(), window->GetHWND());

	testParticles->SetTexture(renderer->GetDevice(), L"Textures/fire.png");
	testParticles->SetMaxParticles(200);
	//testParticles->SetParticleColor(dx::XMFLOAT4(fireRedColor));
	testParticles->SetParticleSize(dx::XMFLOAT2(0.8f, 0.8f));
	//testParticles->SetEmitPos(dx::XMFLOAT3(28, 1.7f, 50));
	testParticles->SetEmitDir(dx::XMFLOAT3(0.0f, 0.5f, 0.0f));
	testParticles->SetParticleSpreadMulti(dx::XMFLOAT3(0.15f, 0.3f, 0.15f));

	testParticles->InitializeParticles(renderer->GetDevice(), renderer, fireLight);
	renderer->ListParticle(testParticles);


	Object* windowLight = new Object("windowLight");
	windowLight->GetTransform().SetPosition({ 3.0f, -98.f, 3 });
	LightComponent* wLight1 = windowLight->AddComponent<LightComponent>(LightType::POINT_LIGHT, dx::XMFLOAT4(0.3f, 0.41f, 0.8f, 1.0f), 5.0f);
	wLight1->SetEnabled(true);
	AddObjectToRoot(windowLight);

	Object* windowLight2 = new Object("windowLight2");
	windowLight2->GetTransform().SetPosition({ -7, -98.f, 3 });
	LightComponent* wLight2 = windowLight2->AddComponent<LightComponent>(LightType::POINT_LIGHT, dx::XMFLOAT4(0.3f, 0.41f, 0.8f, 1.0f), 5.0f);
	wLight2->SetEnabled(true);
	AddObjectToRoot(windowLight2);

	Object* doorLight = new Object("doorLight");
	doorLight->GetTransform().SetPosition({ 2.f, -97.f, -3.3f });
	LightComponent* drLight = doorLight->AddComponent<LightComponent>(LightType::POINT_LIGHT, dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), 7.0f);
	drLight->SetEnabled(true);
	drLight->SetIntensity(0.5);
	AddObjectToRoot(doorLight);

	Object* tableLight = new Object("tableLight");
	tableLight->GetTransform().SetPosition({ -4.f, -98.f, -4.f });
	LightComponent* tblLight = tableLight->AddComponent<LightComponent>(LightType::POINT_LIGHT, dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f), 7.0f);
	tblLight->SetEnabled(true);
	tblLight->SetIntensity(0.5);
	AddObjectToRoot(tableLight);

}

void GameScene::OnActivate()
{
	house->GetComponent<NodeWalkerComp>()->currentNode = 1;
	SaveState& state = SaveHandler::LoadOrCreate();

	LightManager::Instance().ForceUpdateBuffers(renderer->GetContext(), camera);

	Input::Instance().ConfineMouse();
	Input::Instance().SetMouseMode(dx::Mouse::Mode::MODE_RELATIVE);
	ShowCursor(false);

	world.ConstructSegment(state);

	//PrintSceneHierarchy(root, 0);
	house->GetComponent<NodeWalkerComp>()->InitializePath(world.GetPath());
	house->GetComponent<NodeWalkerComp>()->SetWorld(&world);

	if (house != nullptr && player != nullptr)
	{
		std::vector<dx::XMINT2> indexes = world.GetPath().GetIndexes();
		dx::XMINT2 spawnIndex = indexes[0];


		//dx::XMVECTOR position = dx::XMVectorAdd(Chunk::IndexToWorld(spawnIndex, 0.0f), dx::XMVectorSet(CHUNK_SIZE / 2.0f, 0, CHUNK_SIZE / 2.0f, 0));
		//house->GetTransform().SetPosition(position);

	/*	if (house->HasComponent<RigidBodyComponent>())
			house->GetComponent<RigidBodyComponent>()->SetPosition(position);*/



		if (!Engine::Instance->start)
		{
			sm::Vector3 housePos = house->GetTransform().GetLocalPosition();
			fogCol += 0.5f;
			renderer->SetIdAndColor(state.segment, fogCol);

			dx::XMFLOAT3 switchPosition;
			switchPosition = dx::XMFLOAT3{ housePos.x + 10 , 7.f , housePos.z };

			dx::XMVECTOR playerPos = { switchPosition.x, switchPosition.y, switchPosition.z };

			dx::XMVECTOR temphousePos = house->GetTransform().GetLocalPosition();


			//position = dx::XMVectorAdd(dx::XMVECTOR({ 0.0f, 1.0f, 5.0f, 0.0f }), position);

			//player->GetComponent<PlayerComp>()->SetStartPosition(position);

			house->GetTransform().SetPosition({ housePos.x, 3.0f, housePos.z });

			player->GetTransform().SetPosition(playerPos);
			player->GetComponent<RigidBodyComponent>()->SetPosition(playerPos);
		}
		// N�N M�STE FIXA DETTA. JAG PALLAR INTE
		// N�N M�STE FIXA DETTA. JAG PALLAR INTE
		// N�N M�STE FIXA DETTA. JAG PALLAR INTE 
		// N�N M�STE FIXA DETTA. JAG PALLAR INTE
		// N�N M�STE FIXA DETTA. JAG PALLAR INTE
		else if (Engine::Instance->start)

		{

			player->GetComponent<PlayerComp>()->SetStatsFromState(state);
			fogCol = 0.0f;
			renderer->SetIdAndColor(state.segment, fogCol);

			dx::XMVECTOR playerPos = { this->interiorPosition.x, this->interiorPosition.y + 3.0f, this->interiorPosition.z, 0.0f };

			dx::XMVECTOR position = house->GetTransform().GetLocalPosition();
			position = dx::XMVectorAdd(dx::XMVECTOR({ 0.0f, 1.0f, 5.0f, 0.0f }), position);

			player->GetComponent<PlayerComp>()->SetStartPosition(position);

			player->GetTransform().SetPosition(playerPos);
			player->GetComponent<RigidBodyComponent>()->SetPosition(playerPos);

			player->GetComponent<ControllerComp>()->SetInside(true);

			Engine::Instance->start = false;

		}

	}

	renderer->AddRenderPass(guiManager);

	//this->PrintSceneHierarchy(root, 0);
	//enemyManager->SpawnEnemies();

	AudioMaster::Instance().PlaySoundEvent("wind");

	/* Ugly solution */
	player->GetComponent<PlayerComp>()->GetArms()->GetComponent< PlayerAnimHandlerComp>()->SetStarted(true);

	//Place signs
	SetSignPositions(state);

	sceneSwitch = false;
	delayTimer = 0.0f;
	physicsDelay = 5.0f;
	//std::cout << "Game Scene activated " << std::endl;
	guiManager->GetGUIObject("loading")->SetVisible(true);
	//house->GetComponent<NodeWalkerComp>()->canWalk = true;
	house->GetComponent<NodeWalkerComp>()->Reset();
}

void GameScene::OnDeactivate()
{
	player->RemoveFlag(ObjectFlag::ENABLED);
	firstFrame = false;
	sceneSwitch = true;
	delayTimer = 0.0f;

	physicsDelay = 1000.0f;
	guiManager->GetGUIObject("loading")->SetVisible(true);
	//std::cout << "Game Scene deactivated" << std::endl;
	world.DeconstructSegment();
	renderer->RemoveRenderPass(guiManager);
	enemyManager->DespawnEnemies();

	//renderer->ClearParticles();

	ShowCursor(true);
	//this->PrintSceneHierarchy(root, 0);
	player->GetComponent<PlayerComp>()->GetArms()->GetComponent< PlayerAnimHandlerComp>()->SetStarted(false);
}

void GameScene::SetSignPositions(SaveState& state)
{
	
	if (state.segment == 7)
	{
		end = true;
		dx::XMFLOAT3 signPosition;
		signPosition = dx::XMFLOAT3{ world.GetPath().GetSignPosition().x , 1.0f ,world.GetPath().GetSignPosition().y };

		roadSign->GetTransform().SetPosition({ signPosition.x, signPosition.y - 1.0f, signPosition.z });
		roadSign->AddFlag(ObjectFlag::ENABLED);

		rightSign->RemoveFlag(ObjectFlag::ENABLED);
		leftSign->RemoveFlag(ObjectFlag::ENABLED);

		/*std::vector<dx::XMINT2> indexes = world.GetPath().GetIndexes();
		dx::XMINT2 spawnIndex = indexes[0];
		roadSign->GetTransform().SetPosition(dx::XMVectorAdd(Chunk::IndexToWorld(spawnIndex, 0.0f), dx::XMVectorSet(CHUNK_SIZE / 2.0f, 1, CHUNK_SIZE / 2.0f, 0)));*/

	}
	else
	{
		dx::XMFLOAT3 signPosition;
		signPosition = dx::XMFLOAT3{ world.GetPath().GetSignPosition().x , 1.5f ,world.GetPath().GetSignPosition().y };

		//Right Sign
		rightSign->GetTransform().SetPosition({ signPosition.x + 1.0f, signPosition.y - 1.0f, signPosition.z });
		rightSign->GetComponent<RigidBodyComponent>()->SyncWithTransform();
		rightSign->GetComponent<SelectableComponent>()->SetActive(false);

		//Left Sign
		leftSign->GetTransform().SetPosition({ signPosition.x - 1.0f, signPosition.y - 1.0f, signPosition.z });
		leftSign->GetComponent<RigidBodyComponent>()->SyncWithTransform();
		leftSign->GetComponent<SelectableComponent>()->SetActive(false);
	}
}

void GameScene::Update(const float& deltaTime)
{
	if ((delayTimer > (physicsDelay + loadScreenDelay + 2)) && onceTest)
	{
		onceTest = false;
		house->GetComponent<NodeWalkerComp>()->Reset();
	}
	Scene::Update(deltaTime);
	world.UpdateRelevantChunks(player->GetTransform(), camera);
	//world.DrawDebug();

	enemyManager->SpawnRandomEnemy(deltaTime);

	//if (KEY_DOWN(X))
	//	std::cout << "pos: " << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << std::endl;

	// Something CP with controllerComp/player wont allow this to happen inside the playerComp
	if (player->GetComponent<ControllerComp>()->GetInRange())
	{
		guiManager->GetGUIObject("door")->SetVisible(true);
		guiManager->GetGUIObject("dot")->SetVisible(false);
	}
	else
	{
		guiManager->GetGUIObject("door")->SetVisible(false);
	}

	//Left or right
	if (rightSign->GetComponent<SelectableComponent>()->GetActive())
	{
		//set first frame till false

		SaveState state = SaveHandler::LoadOrCreate();
		state.segment++;
		SaveHandler::Save(state);
		//std::cout << "added +1 to segment in save" << std::endl;

		OnDeactivate();
		ShowCursor(false);
		OnActivate();
		rightSign->GetComponent<SelectableComponent>()->SetActive(false);
	}
	else if (leftSign->GetComponent<SelectableComponent>()->GetActive())
	{
		SaveState state = SaveHandler::LoadOrCreate();
		state.segment++;
		SaveHandler::Save(state);

		//std::cout << "added +1 to segment in save" << std::endl;

		OnDeactivate();
		ShowCursor(false);
		OnActivate();
		leftSign->GetComponent<SelectableComponent>()->SetActive(false);
	}

	//Win
	if (end)
	{
		if (roadSign->GetComponent<SelectableComponent>()->GetActive())
		{
			Engine::Instance->SwitchScene(SceneIndex::WIN);
		}
	}
	
	static_cast<GUIFont*>(guiManager->GetGUIObject("vramSprite"))->SetString("vram: " + std::to_string(VramUsage()) + " mb");
	static_cast<GUIFont*>(guiManager->GetGUIObject("ramSprite"))->SetString("ram: " + std::to_string(RamUsage()) + " mb");
	//std::cout << "PlayerPos: " << player->GetTransform().GetPosition().m128_f32[0] << " " << player->GetTransform().GetPosition().m128_f32[1] << " " << player->GetTransform().GetPosition().m128_f32[2] << std::endl;

	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString("fps: " + std::to_string((int)GameClock::Instance().GetFramesPerSecond()));
	//dx::XMFLOAT3 playerPosF;
	//dx::XMStoreFloat3(&playerPosF, player->GetTransform().GetPosition());
	//static_cast<GUIFont*>(guiManager->GetGUIObject("playerPos"))->SetString("Player pos x:" + std::to_string((int)playerPosF.x)
	//+ " y: " + std::to_string((int)playerPosF.z));
	guiManager->UpdateAll();

	/*static float a = 0.0f;
	if (left)
	{
		if (a > -10.0f)
			a -= 0.075f;
		else
		{
			right = true;
			left = false;
		}
	}
	else if (right)
	{
		if (a < 10.0f)
			a += 0.075f;
		else
		{
			right = false;
			left = true;
		}

	}

	testParticles->SetEmitPos(dx::XMFLOAT3(28.0f + a, 1.7f, 50.0f));*/
	dx::XMFLOAT3 eyeCam;
	dx::XMStoreFloat3(&eyeCam, camera->GetOwner()->GetTransform().GetPosition());
	//testParticles->SetEyePos(eyeCam);
	//testParticles->Update(deltaTime, GameClock::Instance().GetSeconds());


	//dx::XMFLOAT3 eyeCam;
	//dx::XMStoreFloat3(&eyeCam, camera->GetOwner()->GetTransform().GetPosition());
	for (auto i : renderer->GetParticleList())
	{
		i->SetEyePos(eyeCam);
		i->Update(deltaTime, GameClock::Instance().GetSeconds());
	}
	//renderer->GetParticlesys(testParticles);

}

void GameScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
}

void GameScene::Render()
{
	camera->UpdateView();

	root->Draw(renderer, camera);
	//worldGenerator.DrawShapes();
	//world.DrawDebug();

	//renderer->GetParticlesys(testParticles);
	//renderer->GetContext().
	//testParticles->Draw(renderer->GetContext(), camera);

	renderer->RenderFrame(camera, (float)clock.GetSeconds(), player->GetComponent<PlayerComp>()->GetDangerDistance());
	firstFrame = true;
}


#if USE_IMGUI
void GameScene::OnIMGUIFrame()
{

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	ImGui::Checkbox("Immortal", &immortal);
	ImGui::Checkbox("Draw Colliders", &drawColliders);

	if (ImGui::Button("Kill player"))
	{
		player->GetComponent<PlayerComp>()->LoseHealth(200.0f);
	}

	if (ImGui::Button("Reset player position"))
	{
		std::cout << "RESETTINGS PLAYER" << std::endl;
		dx::XMFLOAT3 playerPos;
		dx::XMStoreFloat3(&playerPos, player->GetTransform().GetWorldPosition());
		playerPos.x = 0.0f;
		playerPos.z = 0.0f;
		playerPos.y = 20.0f;
		player->GetComponent<RigidBodyComponent>()->SetPosition(dx::XMLoadFloat3(&playerPos));
		player->GetTransform().SetWorldPosition(dx::XMLoadFloat3(&playerPos));
	}

	if (ImGui::Button("Print scene"))
	{
		this->PrintSceneHierarchy(root, 0);
	}

	if (ImGui::Button("Print player position"))
	{
		dx::XMFLOAT3 playerPos;
		dx::XMStoreFloat3(&playerPos, player->GetTransform().GetWorldPosition());
		std::cout << "player current position: " << playerPos.x << ", " << playerPos.y << ", " << playerPos.z << std::endl;

		dx::XMFLOAT3 housePos;
		dx::XMStoreFloat3(&housePos, house->GetTransform().GetWorldPosition());
		std::cout << "house current position: " << housePos.x << ", " << housePos.y << ", " << housePos.z << std::endl;

	}



	if (ImGui::Button("EASY WIN BBY"))
	{
		SaveState state = SaveHandler::LoadOrCreate();
		state.segment++;
		SaveHandler::Save(state);

		OnDeactivate();
		ShowCursor(false);
		OnActivate();
	}
}
#endif
float GameScene::VramUsage()
{
	IDXGIFactory* dxgifactory = nullptr;
	HRESULT ret_code = ::CreateDXGIFactory(
		__uuidof(IDXGIFactory),
		reinterpret_cast<void**>(&dxgifactory));

	float memoryUsage = 0;

	if (SUCCEEDED(ret_code))
	{
		IDXGIAdapter* dxgiAdapter = nullptr;

		if (SUCCEEDED(dxgifactory->EnumAdapters(0, &dxgiAdapter)))
		{
			IDXGIAdapter4* dxgiAdapter4 = NULL;
			if (SUCCEEDED(dxgiAdapter->QueryInterface(__uuidof(IDXGIAdapter4), (void**)&dxgiAdapter4)))
			{
				DXGI_QUERY_VIDEO_MEMORY_INFO info;

				if (SUCCEEDED(dxgiAdapter4->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info)))
				{
					memoryUsage = float(info.CurrentUsage / 1024.0 / 1024.0); //MiB

					/*char msg[100];
					sprintf_s(msg, "%.2f MiB used", memoryUsage);
					MessageBoxA(0, msg, "VRAM", 0);*/
				};

				dxgiAdapter4->Release();
			}
			dxgiAdapter->Release();
		}
		dxgifactory->Release();
	}

	return memoryUsage;
}

float GameScene::RamUsage()
{
	DWORD currentProcessID = GetCurrentProcessId();

	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, currentProcessID);

	if (NULL == hProcess)
		return 0.0f;

	float memoryUsage = 0;

	PROCESS_MEMORY_COUNTERS pmc{};
	if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc)))
	{
		//PagefileUsage is the:
		//The Commit Charge value in bytes for this process.
		//Commit Charge is the total amount of memory that the memory manager has committed for a running process.

		memoryUsage = float(pmc.PagefileUsage / 1024.0 / 1024.0); //MiB
	}

	CloseHandle(hProcess);

	return memoryUsage;
}
