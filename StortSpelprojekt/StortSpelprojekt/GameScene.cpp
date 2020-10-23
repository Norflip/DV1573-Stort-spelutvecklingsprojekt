#include "GameScene.h"
#include "GUISprite.h"
#include "GUIFont.h"

GameScene::GameScene(ResourceManager* manager) : Scene(manager)
{
	nextScene = GAME;
}

GameScene::~GameScene()
{
}

void GameScene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	windowHeight = window->GetHeight();
	windowWidth = window->GetWidth();

	Physics& physics = Physics::Instance();
	physics.Initialize({ 0, -10.0f, 0 });

	pooler.Register("test_body_cube", 10, [](ResourceManager* resources) {

		Object* object = new Object("fuel");

		object->AddComponent<DebugBoxShapeComponent>();
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, true);

		Physics::Instance().RegisterRigidBody(rd);
		return object;
	});

	//Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	input.SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void GameScene::InitializeObjects()
{
	Physics& physics = Physics::Instance();
	
	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);

	SaveState state;
	state.seed = 1337;
	state.segment = 0;

	SegmentDescription desc(0, 10, 2);
	desc.directionalSteps = 5;
	desc.maxSteps = 10;

	
	worldGenerator.Initialize(renderer->GetDevice(), resourceManager->GetShaderResource("terrainShader"), resourceManager->GetShaderResource("grassShader"));
	worldGenerator.Generate(state, renderer->GetDevice(), root);
	worldGenerator.InitalizeGrass(renderer->GetDevice(), renderer->GetContext());

	Mesh* mesh1 = resourceManager->GetResource<Mesh>("Test");
	Mesh* mesh2 = resourceManager->GetResource<Mesh>("Test2");
	Mesh* mesh3 = resourceManager->GetResource<Mesh>("Test3");

	Material* material1 = resourceManager->GetResource<Material>("TestMaterial");
	Material* material2 = resourceManager->GetResource<Material>("Test2Material");
	Material* material3 = resourceManager->GetResource<Material>("Test3Material");

	Object* testObject = new Object("test");
	Object* testObject2 = new Object("test2");
	Object* testObject3 = new Object("test3");

	testObject->AddComponent<MeshComponent>(*mesh1, *material1);
	testObject2->AddComponent<MeshComponent>(*mesh2, *material2);
	testObject3->AddComponent<MeshComponent>(*mesh3, *material3);

	dx::XMFLOAT3 miniTranslation = dx::XMFLOAT3(0, -10, 6);
	dx::XMFLOAT3 miniTranslation2 = dx::XMFLOAT3(2, -10, 2);
	dx::XMFLOAT3 miniTranslation3 = dx::XMFLOAT3(-4, -10, -4);
	dx::XMFLOAT3 miniTranslation4 = dx::XMFLOAT3(0.f, -10.f, 0.f);

	testObject->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation));
	testObject2->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation2));
	testObject3->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation3));

	AddObject(testObject2, testObject);
	AddObject(testObject3, testObject2);

	AddObject(testObject);

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);


	//Object* testMesh4 = new Object("test4");
	//testMesh4->AddComponent<NodeWalkerComp>();
	//testMesh4->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation4));
	//testMesh4->AddComponent<MeshComponent>(*mesh1, *material2);
	//AddObject(testMesh4);

	
	//Player & Camera
	dx::XMFLOAT3 playerSpawn = { 10,10,10 };
	dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
	dx::XMVECTOR playerSpawnVec = dx::XMLoadFloat3(&playerSpawn);
	playerObject = new Object("player", ObjectFlag::ENABLED);
	cameraObject = new Object("camera", ObjectFlag::ENABLED);
	//Transform::SetParentChild(playerObject->GetTransform(), cameraObject->GetTransform());
	this->player = playerObject;
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(this->windowWidth, this->windowHeight);
	cameraObject->GetTransform().SetPosition(playerSpawnVec);
	playerObject->GetTransform().SetPosition(playerSpawnVec);

	//Mesh* meshP = resourceManager->GetResource<Mesh>("Test");
	//Material* materialP = resourceManager->GetResource<Material>("TestMaterial");
	//playerObject->AddComponent<MeshComponent>(*meshP, *materialP);
	playerObject->AddComponent<CapsuleColliderComponent>(0.5f, 4.5f, zero);
	physics.MutexLock();
	RigidBodyComponent* rb = playerObject->AddComponent<RigidBodyComponent>(60.f, FilterGroups::PLAYER, FilterGroups::EVERYTHING, true);
	physics.RegisterRigidBody(rb);
	physics.MutexUnlock();
	playerObject->AddComponent<ControllerComp>(cameraObject); /////////////////
	//Transform::SetParentChild(playerObject->GetTransform(),cameraObject->GetTransform());
	PlayerComp* playerComp = playerObject->AddComponent<PlayerComp>(guiManager, 100, 2, 10, 25, 3);

	AddObject(cameraObject, playerObject); ///
	AddObject(playerObject); /////////////////

	


	/* * * * * * * * ** * * * * */

	//SKELETON ANIMATION MODELS
	//Enemy object //comments
	enemy = new Object("enemy");
	dx::XMFLOAT3 enemyTranslation = dx::XMFLOAT3(0, -10, 10);
	enemy->GetTransform().SetPosition(dx::XMLoadFloat3(&enemyTranslation));
	enemy->AddComponent<MeshComponent>(*mesh1, *material1);
	enemy->AddComponent<EnemyStatsComp>(100, 2, 15, 25, 3);
	enemyStatsComp = enemy->GetComponent<EnemyStatsComp>();
	enemy->AddComponent<EnemyAttackComp>(player->GetComponent<PlayerComp>());
	EnemySMComp* stateMachine = enemy->AddComponent<EnemySMComp>(EnemyState::IDLE);
	stateMachine->RegisterState(EnemyState::IDLE, enemy->AddComponent<EnemyIdleComp>());
	stateMachine->RegisterState(EnemyState::PATROL, enemy->AddComponent<EnemyPatrolComp>());
	stateMachine->RegisterState(EnemyState::ATTACK, enemy->AddComponent<EnemyAttackComp>(player->GetComponent<PlayerComp>()));
	AddObject(enemy);

	playerObject->AddComponent<PlayerAttackComp>(enemy);

	bool defaultAnimation = false;
	bool parentAnimation = true;
	Shader* skeletonShader = resourceManager->GetShaderResource("skeletonShader");

	std::vector<Mesh> skeletonMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/baseMonster.ZWEB", renderer->GetDevice());
	std::vector<Material> skeletonMat = ZWEBLoader::LoadMaterials("Models/baseMonster.ZWEB", skeletonShader, renderer->GetDevice());

	SkeletonAni skeletonbaseMonsterIdle = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterIdle.ZWEB", skeletonMesh[0].GetBoneIDS(), defaultAnimation);

	SkeletonAni skeletonbaseMonsterWalk = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterWalk.ZWEB", skeletonMesh[0].GetBoneIDS(), defaultAnimation);

	SkeletonAni skeletonbaseMonsterRun = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterRun.ZWEB", skeletonMesh[0].GetBoneIDS(), defaultAnimation);

	SkeletonAni skeletonbaseMonsterAttack = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterAttack.ZWEB", skeletonMesh[0].GetBoneIDS(), defaultAnimation);

	SkeletonAni skeletonbaseMonsterDeath = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterDeath.ZWEB", skeletonMesh[0].GetBoneIDS(), defaultAnimation);

	Object* baseMonsterObject = new Object("baseMonster");

	//LOADING BASE MONSTER; ADDING SKELETONS TO IT


	SkeletonMeshComponent* baseMonsterComp = baseMonsterObject->AddComponent<SkeletonMeshComponent>(skeletonMesh[0], skeletonMat[0]);

	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterIdle, SkeletonStateMachine::IDLE);

	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterWalk, SkeletonStateMachine::WALK);

	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterRun, SkeletonStateMachine::RUN);

	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterAttack, SkeletonStateMachine::ATTACK);

	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterDeath, SkeletonStateMachine::DEATH);

	baseMonsterComp->BlendAnimations();

	baseMonsterObject->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
	baseMonsterObject->GetTransform().SetPosition({ 0.0f, 2.5f, 10.0f });

	enemyStatsComp = baseMonsterObject->AddComponent<EnemyStatsComp>(100, 2, 15, 25, 3);

	EnemySMComp* stateMachine = baseMonsterObject->AddComponent<EnemySMComp>(EnemyState::PATROL);

	stateMachine->RegisterState(EnemyState::IDLE, baseMonsterObject->AddComponent<EnemyIdleComp>());
	stateMachine->RegisterState(EnemyState::PATROL, baseMonsterObject->AddComponent<EnemyPatrolComp>());
	stateMachine->RegisterState(EnemyState::ATTACK, baseMonsterObject->AddComponent<EnemyAttackComp>(playerComp));
	stateMachine->InitAnimation();
	AddObject(baseMonsterObject);

	//LOADING HOUSE AND LEGS AND ADDING SKELETONS TO THEM THE HOUSE ONLY HAS ONE JOINT CONNECTED TO IT
	Shader* defaultShader = resourceManager->GetShaderResource("defaultShader");
	std::vector<Mesh> meshHouse = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/House_Base.ZWEB", renderer->GetDevice());
	std::vector<Material> matHouse = ZWEBLoader::LoadMaterials("Models/House_Base.ZWEB", skeletonShader, renderer->GetDevice());

	std::vector<Mesh> skeletonMeshHouseLegs = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/House_Legs.ZWEB", renderer->GetDevice());
	std::vector<Material> skeletonMatHouseLegs = ZWEBLoader::LoadMaterials("Models/House_Legs.ZWEB", skeletonShader, renderer->GetDevice());

	SkeletonAni skeletonHouseBaseIdle = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - IDLE.ZWEB", meshHouse[0].GetBoneIDS(), parentAnimation);
	SkeletonAni skeletonHouseBaseWalk = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - WALK_CYCLE.ZWEB", meshHouse[0].GetBoneIDS(), parentAnimation);
	SkeletonAni skeletonHouseBaseUp = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - STAND_UP.ZWEB", meshHouse[0].GetBoneIDS(), parentAnimation);
	SkeletonAni skeletonHouseBaseDown = ZWEBLoader::LoadSkeletonOnly("Models/House_Base - SIT DOWN - IDLE.ZWEB", meshHouse[0].GetBoneIDS(), parentAnimation);

	SkeletonAni skeletonHouseLegsIdle = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - IDLE.ZWEB", skeletonMeshHouseLegs[0].GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonHouseLegsWalk = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - WALK_CYCLE.ZWEB", skeletonMeshHouseLegs[0].GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonHouseLegsUp = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - STAND_UP.ZWEB", skeletonMeshHouseLegs[0].GetBoneIDS(), defaultAnimation);
	SkeletonAni skeletonHouseLegsDown = ZWEBLoader::LoadSkeletonOnly("Models/House_Legs - SIT DOWN - IDLE.ZWEB", skeletonMeshHouseLegs[0].GetBoneIDS(), defaultAnimation);

	Object* houseBaseObject = new Object("houseBase");

	Object* housesLegsObject = new Object("houseLegs");

	SkeletonMeshComponent* baseComponent = houseBaseObject->AddComponent<SkeletonMeshComponent>(meshHouse[0], matHouse[0]);
	SkeletonMeshComponent* legsComponent = housesLegsObject->AddComponent<SkeletonMeshComponent>(skeletonMeshHouseLegs[0], skeletonMatHouseLegs[0]);

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


	//Character reference
	std::vector<Mesh> charRefMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/char_ref.ZWEB", renderer->GetDevice());
	std::vector<Material> charRefMat = ZWEBLoader::LoadMaterials("Models/char_ref.ZWEB", defaultShader, renderer->GetDevice());

	Object* characterReferenceObject = new Object("characterReference");

	characterReferenceObject->AddComponent<MeshComponent>(charRefMesh[0], charRefMat[0]);
	characterReferenceObject->GetTransform().SetScale({ 1, 1, 1 });
	characterReferenceObject->GetTransform().SetPosition({ 0.0f, -10.0f, 4.0f });
	AddObject(characterReferenceObject);

	//Axe//////////////////////////////////////////////////////////////////
	std::vector<Mesh> axeMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/AXE.ZWEB", renderer->GetDevice());
	std::vector<Material> axeMat = ZWEBLoader::LoadMaterials("Models/AXE.ZWEB", skeletonShader, renderer->GetDevice());

	Object* axeObject = new Object("Axe");

	axeObject->AddComponent<MeshComponent>(axeMesh[0], axeMat[0]);
	axeObject->GetTransform().SetPosition({ 0,0,0 });
	axeObject->GetTransform().SetScale({ 1, 1, 1 });
	axeObject->AddComponent<WeaponComponent>(cameraObject);
	AddObject(axeObject);
	
	clock.Update();
	/* * * * * * * * ** * * * * */
	//Log::Add("PRINTING SCENE HIERARCHY ----");
	//PrintSceneHierarchy(root, 0);
	/*Log::Add("----");*/


	world.Initialize(root, resourceManager, &pooler, renderer);
	world.ConstructSegment(state, desc);
	world.SetPlayer(player);
}

void GameScene::InitializeGUI()
{
	//GUISTUFF//

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
	GUIFont* healthDisplay = new GUIFont(*renderer, "playerHealth", 50, 100);
	GUIFont* enemyDisplay = new GUIFont(*renderer, "enemyHealth", 50, 150);

	//CROSSHAIR
	GUISprite* dot = new GUISprite(*renderer, "Textures/dot.png", (windowWidth / 2) - 6, (windowHeight / 2) - 6, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);
	//GUISprite* crosshair = new GUISprite(*renderer, "Textures/chrosshair.png", (windowWidth / 2) - 25, (windowHeight / 2) - 25, 0, DrawDirection::BottomLeft, ClickFunction::NotClickable);

	// INSERTIONS
	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(healthDisplay, "playerHealth");
	guiManager->AddGUIObject(enemyDisplay, "enemyHealth");

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

	renderer->AddRenderPass(guiManager);
}

void GameScene::InitializeLights()
{
	//TEST POINT LIGHTS____________________________________________________________________________________________________________________
	Object* testPointLight = new Object("testPointLight");
	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(2.0f, -100.0f, 3.0f);
	testPointLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	testPointLight->AddComponent<PointLightComponent>(dx::XMFLOAT4(1.f, 0.f, 0.f, 1.f), 25);
	AddObject(testPointLight);

	Object* testPointLight2 = new Object("testPointLight2");
	dx::XMFLOAT3 lightTranslation2 = dx::XMFLOAT3(0.0f, -100.0f, 3.0f);
	testPointLight2->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation2));
	testPointLight2->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.f, 1.f, 0.f, 1.f), 25);
	//AddObject(testPointLight2);

	Object* testPointLight3 = new Object("testPointLight3");
	dx::XMFLOAT3 lightTranslation3 = dx::XMFLOAT3(-2.0f, -100.0f, 3.0f);
	testPointLight3->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation3));
	testPointLight3->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.f, 0.f, 1.f, 1.f), 25);
	//AddObject(testPointLight3);
	//_____________________________________________________________________________________________________________________________________
}

void GameScene::OnActivate()
{
	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
	nextScene = GAME;
	InitializeLights();
	InitializeGUI();
	InitializeObjects();
}

void GameScene::OnDeactivate()
{
	renderer->RemoveRenderPass(guiManager);
	//worldGenerator.Clear();
	LightManager::Instance().Clear();

	delete root;
	root = nullptr;
}

void GameScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	world.UpdateRelevantChunks();

	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("playerHealth"))->SetString("Player health: " + std::to_string((int)player->GetComponent<PlayerComp>()->GetHealth()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("enemyHealth"))->SetString("Enemy health: " + std::to_string((int)enemyStatsComp->GetHealth()));
	guiManager->UpdateAll();

	if (KEY_DOWN(H))
	{
		dx::XMVECTOR cameraPosition = camera->GetOwner()->GetTransform().GetPosition();

		Physics& phy = Physics::Instance();
		const int ra = 3;

		phy.MutexLock();

		for (int y = -ra; y <= ra; y++)
		{
			for (int x = -ra; x <= ra; x++)
			{
				dx::XMVECTOR position = dx::XMVectorAdd(cameraPosition, { (float)x * 1.5f, -1.0f, (float)y * 1.5f });

				Object* object = new Object("item");
				object->GetTransform().SetPosition(position);

				object->AddComponent<DebugBoxShapeComponent>();
				object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
				RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, true);

				phy.RegisterRigidBody(rd);

				AddObject(object);
			}
		}

		phy.MutexUnlock();

		const int a = 100;
	}

	POINT p = input.GetMousePos();
	Ray ray = camera->MouseToRay(p.x, p.y);
	//std::cout << p.x << ", " << p.y << std::endl;

	if (LMOUSE_PRESSED)
	{
		Physics& phy = Physics::Instance();
		RayHit hit;

		DShape::DrawLine(ray.origin, ray.GetPoint(1000.0f), { 1,1,0 });

		if (phy.RaytestSingle(ray, 1000.0f, hit, FilterGroups::DEFAULT))
		{
			DShape::DrawLine(ray.origin, hit.position, { 1,1,0 });
			DShape::DrawSphere(hit.position, 1.0f, { 0, 0, 1 });

			if (hit.object != nullptr)
			{
				std::cout << hit.object->GetName() << std::endl;
			}
		}
	}
	else
	{
		DShape::DrawSphere(ray.GetPoint(10.0f), 0.2f, { 1, 0, 1 });
	}
	nextScene = NEXT_SCENE(player->GetComponent<PlayerComp>()->GetNextScene());

	//if (player->GetComponent<PlayerComp>()->getNextScene())
	//{
	//	nextScene = LOSE;
	//}
	//else if (KEY_PRESSED(N))
	//{
	//	nextScene = LOSE;
	//}
	//else if (KEY_PRESSED(M))
	//{
	//	nextScene = INTRO;
	//}

	skyboxClass->GetThisObject()->GetTransform().SetPosition(camera->GetOwner()->GetTransform().GetPosition());
}

void GameScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
}

void GameScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	//worldGenerator.DrawShapes();
	//world.DrawDebug();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}


