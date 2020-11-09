#include "stdafx.h"
#include "GameScene.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"

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

	/* For physics/ rigidbody pickup stuff */


	//SKELETON ANIMATION MODELS
	bool defaultAnimation = false;
	bool parentAnimation = true;
	Shader* skeletonShader = resources->GetShaderResource("skeletonShader");

	std::vector<Mesh*> skeletonMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/baseMonster.ZWEB", renderer->GetDevice());
	std::vector<Material*> skeletonMat = ZWEBLoader::LoadMaterials("Models/baseMonster.ZWEB", skeletonShader, renderer->GetDevice());

	SkeletonAni skeletonbaseMonsterIdle = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterIdle.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterWalk = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterWalk.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterRun = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterRun.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterAttack = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterAttack.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonbaseMonsterDeath = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterDeath.ZWEB", skeletonMesh[0]->GetBoneIDS(), defaultAnimation);

	//LOADING HOUSE AND LEGS AND ADDING SKELETONS TO THEM THE HOUSE ONLY HAS ONE JOINT CONNECTED TO IT
	Shader* defaultShader = resources->GetShaderResource("defaultShader");
	Shader* skeletonAlphaShader = resources->GetShaderResource("houseShader");

	std::vector<Mesh*> meshHouse = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/House_Base.ZWEB", renderer->GetDevice());
	std::vector<Material*> matHouse = ZWEBLoader::LoadMaterials("Models/House_Base.ZWEB", skeletonAlphaShader, renderer->GetDevice());

	std::vector<Mesh*> skeletonMeshHouseLegs = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/House_Legs.ZWEB", renderer->GetDevice());
	std::vector<Material*> skeletonMatHouseLegs = ZWEBLoader::LoadMaterials("Models/House_Legs.ZWEB", skeletonShader, renderer->GetDevice());

	SkeletonAni skeletonHouseBaseIdle = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - IDLE.ZWEB", meshHouse[0]->GetBoneIDS(), parentAnimation);
	SkeletonAni skeletonHouseBaseWalk = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - WALK_CYCLE.ZWEB", meshHouse[0]->GetBoneIDS(), parentAnimation);
	SkeletonAni skeletonHouseBaseUp = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - STAND_UP.ZWEB", meshHouse[0]->GetBoneIDS(), parentAnimation);
	SkeletonAni skeletonHouseBaseDown = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - SIT DOWN - IDLE.ZWEB", meshHouse[0]->GetBoneIDS(), parentAnimation);

	SkeletonAni skeletonHouseLegsIdle = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - IDLE.ZWEB", skeletonMeshHouseLegs[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonHouseLegsWalk = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - WALK_CYCLE.ZWEB", skeletonMeshHouseLegs[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonHouseLegsUp = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - STAND_UP.ZWEB", skeletonMeshHouseLegs[0]->GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonHouseLegsDown = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - SIT DOWN - IDLE.ZWEB", skeletonMeshHouseLegs[0]->GetBoneIDS(), defaultAnimation);

	Object* houseBaseObject = new Object("houseBase");
	Object* housesLegsObject = new Object("houseLegs");
	houseBaseObject->GetTransform().Rotate(0, -90.0f, 0.0);
	house = houseBaseObject;

	houseBaseObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(2.5, 5, 2.5), dx::XMFLOAT3(0, 0, 0));
	houseBaseObject->AddComponent<RigidBodyComponent>(0.0f, FilterGroups::PROPS, FilterGroups::EVERYTHING, BodyType::STATIC, true);

	SkeletonMeshComponent* baseComponent = houseBaseObject->AddComponent<SkeletonMeshComponent>(meshHouse[0], matHouse[0], 0.1f);
	SkeletonMeshComponent* legsComponent = housesLegsObject->AddComponent<SkeletonMeshComponent>(skeletonMeshHouseLegs[0], skeletonMatHouseLegs[0], 0.1f);
	
	legsComponent->SetAnimationTrack(skeletonHouseLegsIdle, SkeletonStateMachine::IDLE);
	legsComponent->SetAnimationTrack(skeletonHouseLegsWalk, SkeletonStateMachine::WALK);
	legsComponent->SetAnimationTrack(skeletonHouseLegsUp, SkeletonStateMachine::UP);
	legsComponent->SetAnimationTrack(skeletonHouseLegsDown, SkeletonStateMachine::DOWN);

	baseComponent->SetAnimationTrack(skeletonHouseBaseIdle, SkeletonStateMachine::IDLE);
	baseComponent->SetAnimationTrack(skeletonHouseBaseWalk, SkeletonStateMachine::WALK);
	baseComponent->SetAnimationTrack(skeletonHouseBaseUp, SkeletonStateMachine::UP);
	baseComponent->SetAnimationTrack(skeletonHouseBaseDown, SkeletonStateMachine::DOWN);

	Transform::SetParentChild(baseComponent->GetOwner()->GetTransform(), legsComponent->GetOwner()->GetTransform());
	baseComponent->GetOwner()->GetTransform().SetScale({ 0.5f, 0.5f, 0.5f });

	NodeWalkerComp* nodeWalker = houseBaseObject->AddComponent<NodeWalkerComp>();
	nodeWalker->InitAnimation();

	legsComponent->SetTrack(SkeletonStateMachine::IDLE, false);
	baseComponent->SetTrack(SkeletonStateMachine::IDLE, false);
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

	//Transform::SetParentChild(playerObject->GetTransform(),cameraObject->GetTransform());
	playerObject->AddComponent<PlayerComp>(renderer, camera, Engine::Instance->GetPhysics(), guiManager, 100.f, 2.f, 20.f, 50.f, 3.f);
	playerObject->AddComponent<ControllerComp>(cameraObject, houseBaseObject); /////////////////


	AddObject(cameraObject, playerObject);
	AddObject(playerObject);

	Object* testPointLight = new Object("body_pointLight");

	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	testPointLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	testPointLight->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f), 10.f);

	AddObject(testPointLight, playerObject);

	//LOADING BASE MONSTER; ADDING SKELETONS TO IT
	enemy = new Object("enemy", ObjectFlag::ENABLED);
	SkeletonMeshComponent* baseMonsterComp = enemy->AddComponent<SkeletonMeshComponent>(skeletonMesh[0], skeletonMat[0]);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterIdle, SkeletonStateMachine::IDLE);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterWalk, SkeletonStateMachine::WALK);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterRun, SkeletonStateMachine::RUN);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterAttack, SkeletonStateMachine::ATTACK);
	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterDeath, SkeletonStateMachine::DEATH);
	baseMonsterComp->BlendAnimations();

	//Enemy object //comments
	dx::XMFLOAT3 enemyTranslation = dx::XMFLOAT3(23, 7, 46);
	enemy->GetTransform().SetPosition(dx::XMLoadFloat3(&enemyTranslation));
	enemy->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
	enemy->AddComponent<EnemyStatsComp>(100.f, 2.0f, 10.f, 5.f, 3.f, 3.f);
	//enemyStatsComp = enemy->GetComponent<EnemyStatsComp>();
	enemy->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 1, 2.45, 1 }, dx::XMFLOAT3{ 0, 0, 0 });

	enemy->AddComponent<RigidBodyComponent>(10.f, FilterGroups::ENEMIES, (FilterGroups::EVERYTHING & ~FilterGroups::PICKUPS) & ~FilterGroups::HOLDABLE, BodyType::KINEMATIC, true);
	
	EnemySMComp* stateMachine = enemy->AddComponent<EnemySMComp>(EnemyState::IDLE);
	stateMachine->RegisterState(EnemyState::IDLE, enemy->AddComponent<EnemyIdleComp>());
	//stateMachine->RegisterState(EnemyState::PATROL, enemy->AddComponent<EnemyPatrolComp>());
	stateMachine->RegisterState(EnemyState::ATTACK, enemy->AddComponent<EnemyAttackComp>(player->GetComponent<PlayerComp>()));
	stateMachine->Start();
	stateMachine->InitAnimation();
	AddObject(enemy);

	//physics.MutexLock();
	//RigidBodyComponent* rbEnemy = enemy->AddComponent<RigidBodyComponent>(0.f, FilterGroups::ENEMIES, FilterGroups::PLAYER, BodyType::STATIC);
	////rbEnemy.
	//physics.RegisterRigidBody(rbEnemy);
	//physics.MutexUnlock();
	playerObject->AddComponent<PlayerAttackComp>(enemy);

	std::vector<Mesh*> axeMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/AXE.ZWEB", renderer->GetDevice());
	std::vector<Material*> axeMat = ZWEBLoader::LoadMaterials("Models/AXE.ZWEB", defaultShader, renderer->GetDevice());
	Object* axeObject = new Object("Axe", ObjectFlag::DEFAULT | ObjectFlag::NO_CULL);

	axeObject->AddComponent<MeshComponent>(axeMesh[0], axeMat[0]);
	axeObject->GetTransform().SetPosition({ 0,0,0 });
	axeObject->GetTransform().SetScale({ 1, 1, 1 });
	axeObject->AddComponent<WeaponComponent>(cameraObject);
		
	AddObject(axeObject);
	playerObject->GetComponent<PlayerComp>()->InsertWeapon(axeObject->GetComponent<WeaponComponent>(), axeObject->GetName());

	world.Initialize(root, resources, pooler, renderer);
	

	dx::XMVECTOR asdf = dx::XMVectorSet(23, 3, 40 ,1); //???
	enemy->GetTransform().SetPosition(asdf);

	/* PICKUP STUFF DONT DELETE THESEEE */
	/* PICKUP STUFF DONT DELETE THESEEE */
	/* PICKUP STUFF DONT DELETE THESEEE */
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
	Object* beansObject = resources->AssembleObject("Soup", "SoupMaterial");
	beansObject->GetTransform().SetPosition({22, 2.0f, 53 });
	beansObject->AddComponent<BoxColliderComponent>(dx::XMFLOAT3{ 0.5f, 0.5f, 0.5f }, dx::XMFLOAT3{ 0, 0, 0 });
	beansObject->AddComponent<PickupComponent>(Type::Food, 20.0f);
	beansObject->AddComponent<RigidBodyComponent>(0.f, FilterGroups::PICKUPS, (FilterGroups::EVERYTHING & ~FilterGroups::PLAYER), BodyType::DYNAMIC, true);
	AddObject(beansObject);
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
	GUISprite* foodScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 10, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 90, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* fuelScalingBar = new GUISprite(*renderer, "Textures/DippingBar.png", 170, 10, 0.5, DrawDirection::BottomRight, ClickFunction::NotClickable);

	//BARS AR RIGHT SIDE
	GUISprite* fuelBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 10, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 90, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthBar = new GUISprite(*renderer, "Textures/Health_Fuel_Food.png", 170, 10, 1, DrawDirection::BottomRight, ClickFunction::NotClickable);

	//ICONS ON TOP OF ITEMS
	GUISprite* equimpmentSpriteAxe = new GUISprite(*renderer, "Textures/AxeIcon2.png", 10, 10, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);

	//ICONS ON TOP OF BARS
	GUISprite* fuelSprite = new GUISprite(*renderer, "Textures/FireIcon.png", 10, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* foodSprite = new GUISprite(*renderer, "Textures/FoodIcon.png", 90, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);
	GUISprite* healthSprite = new GUISprite(*renderer, "Textures/HealthIcon.png", 170, 10, 0, DrawDirection::BottomRight, ClickFunction::NotClickable);

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

	guiManager->AddGUIObject(foodScalingBar, "fuelDippingBar");
	guiManager->AddGUIObject(healthScalingBar, "foodDippingBar");
	guiManager->AddGUIObject(fuelScalingBar, "healthDippingBar");

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

	//this->PrintSceneHierarchy(root, 0);
}

void GameScene::OnDeactivate()
{
	world.DeconstructSegment();
	renderer->RemoveRenderPass(guiManager);

	ShowCursor(true);
	//this->PrintSceneHierarchy(root, 0);
}

void GameScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	world.UpdateRelevantChunks();

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


