#include "stdafx.h"
#include "GameScene.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"
#include "GUICompass.h"
void GameScene::RemoveEnemy()
{
	enemy->RemoveFlag(ObjectFlag::ENABLED);
	enemy->AddFlag(ObjectFlag::REMOVED);
}

GameScene::GameScene()
{
	
}

GameScene::~GameScene()
{
}

void GameScene::Initialize()
{
	pooler->Register("test_body_cube", 10, [](ResourceManager* resources) {

		Object* object = new Object("physics_cube");

		Mesh* mesh1 = resources->GetResource<Mesh>("Test");
		Material* material1 = resources->GetResource<Material>("TestMaterial");

		object->AddComponent<MeshComponent>(mesh1, material1);
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
		object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, BodyType::DYNAMIC, true);
		return object;
	});

	InitializeLights();
	InitializeGUI();
	InitializeObjects();

}

void GameScene::InitializeObjects()
{
	Object* houseBaseObject = new Object("houseBase");
	Object* housesLegsObject = new Object("houseLegs");
	//houseBaseObject->GetTransform().Rotate(0, -90.0f, 0.0);
	houseBaseObject->GetTransform().Rotate(0, -90.0f * Math::ToRadians, 0.0);

	house = houseBaseObject;

	//														Extence					pos
	//WALLS
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(1.5, 3.5, 2.1), dx::XMFLOAT3(0, 4, -1));
	//PORCH
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(3.375, 0.325, 3), dx::XMFLOAT3(0, 1, 0.05));
	//FENCE BACK
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.125, 0.625, 3.375), dx::XMFLOAT3(-3.3, 2, 0.05));
	//FENCE FRONT
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.125, 0.625, 2.25), dx::XMFLOAT3(3.25, 3, 0.7));
	//FENCE RIGHT
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(5.375, 0.625, 0.15), dx::XMFLOAT3(0, 3, -2.75));
	//FENCE LEFT
	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(5.375, 0.625, 0.15), dx::XMFLOAT3(0, 3, 2.75));

	houseBaseObject->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);

	SkeletonMeshComponent* baseComponent = resources->GetResource<SkeletonMeshComponent>("HouseSkeleton"); 
	SkeletonMeshComponent* legsComponent = resources->GetResource<SkeletonMeshComponent>("HouseLegsSkeleton");
	baseComponent->SetTimeScale(0.5f);
	legsComponent->SetTimeScale(0.5f);
	baseComponent->SetTrack(SkeletonStateMachine::IDLE, false);
	legsComponent->SetTrack(SkeletonStateMachine::IDLE, false);

	houseBaseObject->AddComponent<SkeletonMeshComponent>(baseComponent);
	housesLegsObject->AddComponent<SkeletonMeshComponent>(legsComponent);

	Transform::SetParentChild(houseBaseObject->GetTransform(), housesLegsObject->GetTransform());
	houseBaseObject->GetTransform().SetScale({ 0.5f, 0.5f, 0.5f });

	NodeWalkerComp* nodeWalker = houseBaseObject->AddComponent<NodeWalkerComp>();
	nodeWalker->InitAnimation();
	
	AddObject(houseBaseObject);

	//Player & Camera
	dx::XMFLOAT3 playerSpawn = { 10,2,10 };
	dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
	dx::XMVECTOR playerSpawnVec = dx::XMLoadFloat3(&playerSpawn);
	Object* playerObject = new Object("player", ObjectFlag::ENABLED);
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	this->player = playerObject;
	camera = cameraObject->AddComponent<CameraComponent>(window->GetWidth(), window->GetHeight(), 60.0f);
	
	cameraObject->GetTransform().SetPosition(playerSpawnVec);
	playerObject->GetTransform().SetPosition(playerSpawnVec);
	playerObject->AddComponent<CapsuleColliderComponent>(0.5f, 1.8f, zero);
	playerObject->AddComponent<RigidBodyComponent>(60.f, FilterGroups::PLAYER, (FilterGroups::EVERYTHING), BodyType::DYNAMIC, true);

	playerObject->AddComponent<PlayerComp>(renderer, camera, Engine::Instance->GetPhysics(), guiManager, 100.f, 2.f, 20.f, 50.f, 3.f);
	playerObject->AddComponent<ControllerComp>(cameraObject, houseBaseObject); /////////////////

	AddObject(cameraObject, playerObject);
	AddObject(playerObject);

	Object* testPointLight = new Object("body_pointLight");

	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	testPointLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	testPointLight->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), 10.f);

	AddObject(testPointLight, playerObject);


	world.Initialize(root, resources, pooler, renderer, camera);

	//LOADING BASE MONSTER; ADDING SKELETONS TO IT
	enemyManager = new EnemyManager(resources, player, player->GetComponent<PlayerComp>(), root);
	enemyManager->InitBaseEnemy();
	
	/* PICKUP STUFF DONT DELETE THESEEE */
	Object* healthkitObject = resources->AssembleObject("HealthKit", "HealthKitMaterial");
	healthkitObject->GetTransform().SetPosition({ 23,2,50 });
	healthkitObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 0.5f, 0.5f, 0.5f }, dx::XMFLOAT3{ 0, 0, 0 });
	healthkitObject->AddComponent<PickupComponent>(Type::Health, 20.0f);
	healthkitObject->AddComponent<RigidBodyComponent>(0.f, FilterGroups::PICKUPS, (FilterGroups::EVERYTHING &~FilterGroups::PLAYER), BodyType::DYNAMIC,true);
	AddObject(healthkitObject);

	///* Fuel pickup stuff temporary */
	Object* fuelCanObject = resources->AssembleObject("FuelCanGreen", "FuelCanGreenMaterial");

	fuelCanObject->GetTransform().SetPosition({ 22,2,52 });
	fuelCanObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 0.3f, 0.3f, 0.3f }, dx::XMFLOAT3{ 0, 0, 0 });
	fuelCanObject->AddComponent<PickupComponent>(Type::Fuel, 20.0f);
	fuelCanObject->AddComponent<RigidBodyComponent>(10.f, FilterGroups::HOLDABLE, (FilterGroups::EVERYTHING &~FilterGroups::PLAYER), BodyType::DYNAMIC, true);
	AddObject(fuelCanObject);

	///* Banana pickup stuff temporary */
	Shader* particleShader = resources->GetShaderResource("particleShader");
	Object* beansObject = resources->AssembleObject("Soup", "SoupMaterial");
	beansObject->GetTransform().SetPosition({22, 2.0f, 53 });
	beansObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 0.5f, 0.5f, 0.5f }, dx::XMFLOAT3{ 0, 0, 0 });
	beansObject->AddComponent<PickupComponent>(Type::Food, 20.0f);
	beansObject->AddComponent<RigidBodyComponent>(0.f, FilterGroups::PICKUPS, (FilterGroups::EVERYTHING & ~FilterGroups::PLAYER), BodyType::DYNAMIC, true);

	beansObject->AddComponent<ParticleSystemComponent>(renderer, camera, particleShader);
	beansObject->GetComponent<ParticleSystemComponent>()->InitializeParticles(renderer->GetDevice(), L"Textures/starstar.png");
	AddObject(beansObject);		

	beansObject->AddComponent<RigidBodyComponent>(0.f, FilterGroups::PICKUPS, FilterGroups::TERRAIN, BodyType::DYNAMIC, true);
	AddObject(beansObject);

	//Player Arms
	Object* playerArmsObject = new Object("PlayerArms");
	SkeletonMeshComponent* playerAmrsComp = resources->GetResource<SkeletonMeshComponent>("PlayerArmsSkeleton");
	playerArmsObject->AddComponent<SkeletonMeshComponent>(playerAmrsComp);
	playerArmsObject->AddComponent<PlayerAnimHandlerComp>(playerAmrsComp, cameraObject, playerObject);
	//playerArmsObject->GetTransform().SetPosition({ 22, 2.0f, 53 });

	AddObject(playerArmsObject);

	//Object* axeObject = resources->AssembleObject("Axe", "AxeMaterial");
	//axeObject->GetTransform().SetPosition({ 0,0,0 });
	//axeObject->GetTransform().SetScale({ 1, 1, 1 });
	//axeObject->AddComponent<WeaponComponent>(playerAmrsComp);
	//axeObject->AddFlag(ObjectFlag::NO_CULL);
	//AddObject(axeObject);
	//playerObject->GetComponent<PlayerComp>()->InsertWeapon(axeObject->GetComponent<WeaponComponent>(), axeObject->GetName());



	//std::vector<Mesh*> playerMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/Player_Arms.ZWEB", renderer->GetDevice());
	//std::vector<Material*> playerMat = ZWEBLoader::LoadMaterials("Models/Player_Arms.ZWEB", skeletonShader, renderer->GetDevice());
	////playerArmsObject->AddComponent<MeshComponent>(playerMesh[0], playerMat[0]);

	//SkeletonAni skeletonPlayer[4];
	//skeletonPlayer[0] = ZWEBLoader::LoadSkeletonOnly("Models/Player_Idle.ZWEB", playerMesh[0]->GetBoneIDS(), defaultAnimation);
	//skeletonPlayer[1] = ZWEBLoader::LoadSkeletonOnly("Models/Player_Walk_Cycle.ZWEB", playerMesh[0]->GetBoneIDS(), defaultAnimation);
	//skeletonPlayer[2] = ZWEBLoader::LoadSkeletonOnly("Models/Player_Run.ZWEB", playerMesh[0]->GetBoneIDS(), defaultAnimation);
	//skeletonPlayer[3] = ZWEBLoader::LoadSkeletonOnly("Models/Player_Attack.ZWEB", playerMesh[0]->GetBoneIDS(), defaultAnimation);

	//SkeletonMeshComponent* playerComp = playerArmsObject->AddComponent<SkeletonMeshComponent>(playerMesh[0], playerMat[0]);
	//playerComp->SetAnimationTrack(skeletonPlayer[0], SkeletonStateMachine::IDLE);
	//playerComp->SetAnimationTrack(skeletonPlayer[1], SkeletonStateMachine::WALK);
	//playerComp->SetAnimationTrack(skeletonPlayer[2], SkeletonStateMachine::RUN);
	//playerComp->SetAnimationTrack(skeletonPlayer[3], SkeletonStateMachine::ATTACK);
	//playerComp->BlendAnimations();

	//playerArmsObject->GetTransform().SetPosition({ 22, 2.0f, 53 });

	

	

	//AXE

	/*std::vector<Mesh*> axeMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/AXE.ZWEB", renderer->GetDevice());
	std::vector<Material*> axeMat = ZWEBLoader::LoadMaterials("Models/AXE.ZWEB", defaultShader, renderer->GetDevice());
	Object* axeObject = new Object("Axe", ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);

	axeObject->AddComponent<MeshComponent>(axeMesh[0], axeMat[0]);
	axeObject->GetTransform().SetPosition({ 22, 2.0f, 53 });
	axeObject->GetTransform().SetScale({ 1, 1, 1 });

	axeObject->AddComponent<WeaponComponent>(playerComp);*/

	//AddObject(axeObject);

	//playerObject->GetComponent<PlayerComp>()->InsertWeapon(axeObject->GetComponent<WeaponComponent>(), axeObject->GetName());

}

void GameScene::InitializeGUI()
{
	float windowWidth = FCAST(window->GetWidth());
	float windowHeight = FCAST(window->GetHeight());
	
	//INFO, WE ARE DRAWING BACK TO FRONT. IF YOU WANT SOMETHING TO BE IN FRONT. SET VALUE TO 0. IF YOU WANT IT IN BACK USE 0.1 -> 1

	//BUTTONS AT LEFT SIDE
	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* equimpmentSprite1 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 10, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* equimpmentSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 90, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* equimpmentSprite3 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 170, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	GUISprite* equimpmentSprite4 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 250, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);

	//BARS THAT SCALING 
	GUISprite* fuelScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 170, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 90, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 10, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);

	//BARS AR RIGHT SIDE
	GUISprite* fuelBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 170, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 90, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 10, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);

	//ICONS ON TOP OF ITEMS
	GUISprite* equimpmentSpriteAxe = new GUISprite(*renderer, "Textures/AxeIcon2.png", 10, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	//ICONS ON TOP OF BARS
	GUISprite* fuelSprite = new GUISprite(*renderer, "Textures/FireIcon.png", 170, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodSprite = new GUISprite(*renderer, "Textures/FoodIcon.png", 90, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthSprite = new GUISprite(*renderer, "Textures/HealthIcon.png", 10, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);

	//COMPASS
	GUICompass* compass = new GUICompass(*renderer, window);
	//FONTS
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	//GUIFont* healthDisplay = new GUIFont(*renderer, "playerHealth", 50, 100);
	//GUIFont* enemyDisplay = new GUIFont(*renderer, "enemyHealth", 50, 150);

	//CROSSHAIR
	GUISprite* dot = new GUISprite(*renderer, "Textures/TestFix2.png", (windowWidth / 2) - 6, (windowHeight / 2) - 6, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	//GUISprite* crosshair = new GUISprite(*renderer, "Textures/Crosshair.png", (windowWidth / 2) - 25, (windowHeight / 2) - 25, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);

	// INSERTIONS
	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(fpsDisplay, "fps");
	//guiManager->AddGUIObject(healthDisplay, "playerHealth");
	//guiManager->AddGUIObject(enemyDisplay, "enemyHealth");

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

	//CROSSHAIR
	guiManager->AddGUIObject(dot, "dot");
	//guiManager->AddGUIObject(crosshair, "crosshair");
}

void GameScene::InitializeLights()
{
	//TEST POINT LIGHTS____________________________________________________________________________________________________________________
	/*Object* testPointLight = new Object("testPointLight");
	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(2.0f, 0.0f, 3.0f);
	testPointLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	testPointLight->AddComponent<PointLightComponent>(dx::XMFLOAT4(1.f, 0.f, 0.f, 1.f), 25);
	AddObject(testPointLight);*/

	//Object* testPointLight2 = new Object("testPointLight2");
	//dx::XMFLOAT3 lightTranslation2 = dx::XMFLOAT3(0.0f, 2.0f, 3.0f);
	//testPointLight2->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation2));
	//testPointLight2->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.f, 1.f, 0.f, 1.f), 25);
	////AddObject(testPointLight2);

	//Object* testPointLight3 = new Object("testPointLight3");
	//dx::XMFLOAT3 lightTranslation3 = dx::XMFLOAT3(-2.0f, 0.0f, 3.0f);
	//testPointLight3->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation3));
	//testPointLight3->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.f, 0.f, 1.f, 1.f), 25);
	////AddObject(testPointLight3);
	//_____________________________________________________________________________________________________________________________________
}

void GameScene::OnActivate()
{
	SaveState state;
	state.seed = 1337;
	state.segment = 0;

	SegmentDescription desc(0, 10, 2);
	desc.directionalSteps = 1;
	desc.maxSteps = 10;

	player->GetComponent<PlayerComp>()->Reset();
	world.ConstructSegment(state, desc);
	world.SetPlayer(player);
	world.SetHouse(house);

	house->GetComponent<NodeWalkerComp>()->InitializePath(world.GetPath());
	world.MoveHouseAndPlayerToStart();




	renderer->AddRenderPass(guiManager);

	
	Input::Instance().ConfineMouse();
	Input::Instance().SetMouseMode(dx::Mouse::Mode::MODE_RELATIVE);
	ShowCursor(false);



	AudioMaster::Instance().PlaySoundEvent("wind");
	//this->PrintSceneHierarchy(root, 0);
}

void GameScene::OnDeactivate()
{
	world.DeconstructSegment();
	renderer->RemoveRenderPass(guiManager);
	
	//renderer->ClearParticles();

	ShowCursor(true);
	//this->PrintSceneHierarchy(root, 0);
}

void GameScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	world.UpdateRelevantChunks();
	//world.DrawDebug();

	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));
	guiManager->UpdateAll();
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
	
	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}


