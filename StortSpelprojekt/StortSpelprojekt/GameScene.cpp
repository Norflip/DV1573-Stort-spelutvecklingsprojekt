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

	pooler.Register("test_body_cube", 10, []() {

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

	worldGenerator.Initialize(renderer->GetDevice(), resourceManager->GetShaderResource("terrainShader"), resourceManager->GetShaderResource("grassShader"));
	worldGenerator.Generate(state, renderer->GetDevice(), root);
	worldGenerator.InitalizeGrass(renderer->GetDevice(), renderer->GetContext());

	
	/*************************INSTANCING*******************/
	auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, renderer->GetDevice());

	Shader* instanceShader = resourceManager->GetShaderResource("instanceShader");
	Shader* alphaInstanceShader = resourceManager->GetShaderResource("alphaInstanceShader");

	//0 base 1 branch 2 leaves
	std::vector<Mesh> stylizedTreeModel = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/tree.ZWEB", renderer->GetDevice());
	//0 tree 1 leaves
	std::vector<Material> stylizedTreeMaterial = ZWEBLoader::LoadMaterials("Models/tree.ZWEB", instanceShader, renderer->GetDevice());


	//TEST POINT LIGHTS____________________________________________________________________________________________________________________
	Object* testPointLight = new Object("testPointLight");
	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(2.0f, 0.0f, 3.0f);
	testPointLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	testPointLight->AddComponent<PointLightComponent>(dx::XMFLOAT4(1.f, 0.f, 0.f, 1.f), 25);
	AddObject(testPointLight);

	Object* testPointLight2 = new Object("testPointLight2");
	dx::XMFLOAT3 lightTranslation2 = dx::XMFLOAT3(0.0f, 2.0f, 3.0f);
	testPointLight2->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation2));
	testPointLight2->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.f, 1.f, 0.f, 1.f), 25);
	AddObject(testPointLight2);

	Object* testPointLight3 = new Object("testPointLight3");
	dx::XMFLOAT3 lightTranslation3 = dx::XMFLOAT3(-2.0f, 0.0f, 3.0f);
	testPointLight3->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation3));
	testPointLight3->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.f, 0.f, 1.f, 1.f), 25);

	AddObject(testPointLight3);

	//_____________________________________________________________________________________________________________________________________

	stylizedTreeMaterial[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	stylizedTreeMaterial[1].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	stylizedTreeMaterial[0].SetShader(instanceShader);
	stylizedTreeMaterial[1].SetShader(alphaInstanceShader);

	worldGenerator.InitializeTrees(stylizedTreeModel, stylizedTreeMaterial, renderer->GetDevice());
	
	//Player & Camera
	dx::XMFLOAT3 playerSpawn = { 10,20,10 };
	dx::XMFLOAT3 zero = { 0.f, 0.f, 0.f };
	dx::XMVECTOR playerSpawnVec = dx::XMLoadFloat3(&playerSpawn);
	Object* playerObject = new Object("player", ObjectFlag::ENABLED);
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
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
	playerObject->AddComponent<ControllerComp>(cameraObject);
	//Transform::SetParentChild(playerObject->GetTransform(),cameraObject->GetTransform());
	PlayerComp* playerComp = playerObject->AddComponent<PlayerComp>(guiManager, 100, 2, 10, 25, 3);

	AddObject(cameraObject, playerObject);
	AddObject(playerObject);

	


	/* * * * * * * * ** * * * * */

	//SKELETON ANIMATION MODELS

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
	characterReferenceObject->GetTransform().SetPosition({ 0.0f, 0.0f, 4.0f });
	AddObject(characterReferenceObject);


	clock.Update();
	
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

	renderer->AddRenderPass(guiManager);
}

void GameScene::InitializeLights()
{
	//TEST POINT LIGHTS____________________________________________________________________________________________________________________
	Object* testPointLight = new Object("testPointLight");
	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(2.0f, 0.0f, 3.0f);
	testPointLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	testPointLight->AddComponent<PointLightComponent>(dx::XMFLOAT4(1.f, 0.f, 0.f, 1.f), 25);
	AddObject(testPointLight);

	Object* testPointLight2 = new Object("testPointLight2");
	dx::XMFLOAT3 lightTranslation2 = dx::XMFLOAT3(0.0f, 2.0f, 3.0f);
	testPointLight2->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation2));
	testPointLight2->AddComponent<PointLightComponent>(dx::XMFLOAT4(0.f, 1.f, 0.f, 1.f), 25);
	//AddObject(testPointLight2);

	Object* testPointLight3 = new Object("testPointLight3");
	dx::XMFLOAT3 lightTranslation3 = dx::XMFLOAT3(-2.0f, 0.0f, 3.0f);
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
	worldGenerator.Clear();
	LightManager::Instance().Clear();

	delete root;
	root = nullptr;
}

void GameScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);

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
	worldGenerator.DrawShapes();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}


