#include "Scene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
Scene::Scene() : input(Input::Instance())
{
	skyboxClass = nullptr;
	renderer = nullptr;
	camera = nullptr;

	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
}

Scene::~Scene()
{	
	delete skyboxClass;
	skyboxClass = nullptr;

	delete root;
	root = nullptr;	

	if (resourceManager)
		delete resourceManager;
}

void Scene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	// TEMP
	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	Physics& physics = Physics::Instance();
	physics.Initialize({ 0, -10.0f, 0 });


	resourceManager = new ResourceManager;
	resourceManager->InitializeResources(renderer->GetDevice());

	pooler.Register("test_body_cube", 10, []() {

		Object* object = new Object("fuel");

		object->AddComponent<DebugBoxShapeComponent>();
		object->AddComponent<BoxColliderComponent>(dx::XMFLOAT3(0.5f, 0.5f, 0.5f), dx::XMFLOAT3(0, 0, 0));
		RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING, true);

		Physics::Instance().RegisterRigidBody(rd);
		return object;
	});

	spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* normalSprite = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::BottomLeft, ClickFunction::Clickable);
	GUISprite* buttonSprite = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::BottomRight, ClickFunction::Clickable);
	GUISprite* normalSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::TopLeft, ClickFunction::Clickable);
	GUISprite* buttonSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::TopRight, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "test", 300, 300);
	GUIFont* healthDisplay = new GUIFont(*renderer, "playerHealth", 300, 350);
	normalSprite->SetActive();

	guiManager = new GUIManager(renderer);
	
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(healthDisplay, "playerHealth");
	guiManager->AddGUIObject(normalSprite, "normalSprite");
	guiManager->AddGUIObject(buttonSprite, "buttonSprite");
	guiManager->AddGUIObject(normalSprite2, "normalSprite2");
	guiManager->AddGUIObject(buttonSprite2, "buttonSprite2");
	guiManager->GetGUIObject("normalSprite")->SetPosition(100, 100);
	renderer->AddRenderPass(new GUIRenderPass(100, guiManager));

	SaveState state;
	state.seed = 1337;
	state.segment = 0;

	worldGenerator.Initialize(renderer->GetDevice(), resourceManager->GetShaderResource("terrainShader"), resourceManager->GetShaderResource("grassShader"));
	worldGenerator.Generate(state, renderer->GetDevice(), root);
	worldGenerator.InitalizeGrass(renderer->GetDevice(), renderer->GetContext());

	dx::XMFLOAT3 pos = { 10,20,10 };
	dx::XMVECTOR temp = dx::XMLoadFloat3(&pos);
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(window->GetWidth(), window->GetHeight());
	dx::XMFLOAT3 camPos;
	dx::XMStoreFloat3(&camPos,cameraObject->GetTransform().GetPosition());
	cameraObject->GetTransform().SetPosition(temp);
	//cameraObject->AddComponent<CapsuleColliderComponent>(0.5f, 4.5f,camPos);
	//RigidBodyComponent* rd = cameraObject->AddComponent<RigidBodyComponent>(5.f, FilterGroups::PLAYER, FilterGroups::EVERYTHING,false);
	//rd->LockRotation(true);
	//Physics& phy = Physics::Instance();
	//phy.RegisterRigidBody(rd);
	cameraObject->AddComponent<ControllerComponent>();
	cameraObject->GetComponent<ControllerComponent>()->AssignCamComponent(cameraObject->GetComponent<CameraComponent>());
	//cameraObject->GetComponent<ControllerComponent>()->AssignRBComponent(rd);
	cameraObject->AddComponent<StatsComponent>(100, 2, 10, 25, 3);


	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	AddObject(cameraObject);

	InitializeObjects();

	//Log::Add("PRINTING SCENE HIERARCHY ----");
	//PrintSceneHierarchy(root, 0);
	/*Log::Add("----");*/
  
}

void Scene::InitializeObjects()
{
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

	dx::XMFLOAT3 miniTranslation = dx::XMFLOAT3(0, 0, 6);
	dx::XMFLOAT3 miniTranslation2 = dx::XMFLOAT3(2, 2, 2);
	dx::XMFLOAT3 miniTranslation3 = dx::XMFLOAT3(-4, -3, -4);
	dx::XMFLOAT3 miniTranslation4 = dx::XMFLOAT3(0.f, -7.f, 0.f);

	testObject->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation));
	testObject2->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation2));
	testObject3->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation3));

	AddObject(testObject2, testObject);
	AddObject(testObject3, testObject2);

	AddObject(testObject);

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);

	Object* testMesh4 = new Object("test4");
	testMesh4->AddComponent<NodeWalkerComponent>();
	testMesh4->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation4));
	testMesh4->AddComponent<MeshComponent>(*mesh1, *material2);
	AddObject(testMesh4);

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
	
	//Enemy object
	Object* enemy = new Object("enemy");
	dx::XMFLOAT3 enemyTranslation = dx::XMFLOAT3(0, 2, 10);
	enemy->GetTransform().SetPosition(dx::XMLoadFloat3(&enemyTranslation));
	enemy->AddComponent<MeshComponent>(*mesh1, *material1);
	enemy->AddComponent<StatsComponent>(100, 2, 10, 25, 3);
	StateMachineComponent* stateMachine = enemy->AddComponent<StateMachineComponent>(AIState::idle);
	stateMachine->RegisterState(AIState::idle, enemy->AddComponent<AIIdle>());
	stateMachine->RegisterState(AIState::patrol, enemy->AddComponent<AIPatrol>());
	stateMachine->RegisterState(AIState::attack, enemy->AddComponent<AIAttack>(camera));
	AddObject(enemy);

	/* * * * * * * * ** * * * * */

  Shader* skeletonShader = resourceManager->GetShaderResource("skeletonShader");
  
	std::vector<Mesh> skeletonMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/baseMonster.ZWEB", renderer->GetDevice());
	std::vector<Material> skeletonMat = ZWEBLoader::LoadMaterials("Models/baseMonster.ZWEB", skeletonShader, renderer->GetDevice());

	SkeletonAni skeletonbaseMonsterIdle = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterIdle.ZWEB", skeletonMesh[0].GetBoneIDS());


	Object* baseMonsterObject = new Object("baseMonster");

	SkeletonMeshComponent* baseMonsterComp = baseMonsterObject->AddComponent<SkeletonMeshComponent>(skeletonMesh[0], skeletonMat[0]);

	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterIdle, SkeletonStateMachine::IDLE);

	baseMonsterObject->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
	baseMonsterObject->GetTransform().SetPosition({ 0.0f, 2.5f, 0.0f });
	AddObject(baseMonsterObject);

	clock.Update();
	clock.Start();
	clock.Update();

}

void Scene::Update(const float& deltaTime)
{
	clock.Update();

	//dx::XMFLOAT3 positionA = { 0,0,2 };
	//dx::XMFLOAT3 positionB = { 0, 2,-5};

	//DShape::DrawBox(positionA, { 2,2,2 }, { 0, 1, 1 });
	//DShape::DrawWireBox(positionB, { 4,4,4 }, { 1,0,0 });
	//DShape::DrawSphere({ -4,0,0 }, 1.0f, { 0, 0, 1 });
	//DShape::DrawWireSphere({ -4,0,5 }, 1.0f, { 0,1,0 });
	//DShape::DrawLine(positionA, positionB, { 1,1,0 });
  
	input.UpdateInputs();
	
	root->Update(deltaTime);

	skyboxClass->GetThisObject()->GetTransform().SetPosition(camera->GetOwner()->GetTransform().GetPosition());

	GameClock::Instance().Update();
	//std::cout << "FPS: " << GameClock::Instance().GetFramesPerSecond() << std::endl;

	GUIFont* fps = static_cast<GUIFont*>(guiManager->GetGUIObject("fps"));
	fps->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));
	GUIFont* playerHealth = static_cast<GUIFont*>(guiManager->GetGUIObject("playerHealth"));
	playerHealth->SetString(std::to_string((int)camera->GetOwner()->GetComponent<StatsComponent>()->GetHealth()));
	guiManager->UpdateAll();


	float t = (float)clock.GetSeconds();
	t = t;
	if (clock.GetSeconds() > 60)
	{
		clock.Restart();
	}


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

				Object* object = pooler.GetItem("test_body_cube");
				object->GetComponent<RigidBodyComponent>()->SetPosition(position);
				AddObject(object);
			}
		}

		phy.MutexUnlock();
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
	
	// Press P to recompile shaders
	if (KEY_PRESSED(P))
	{
		resourceManager->CompileShaders(renderer->GetDevice());
	}
}

void Scene::FixedUpdate(const float& fixedDeltaTime)
{
	Physics::Instance().FixedUpdate(fixedDeltaTime);
	
	//Log::Add(std::to_string(fixedDeltaTime));
	root->FixedUpdate(fixedDeltaTime);
}

void Scene::Render()
{	
	// skybox draw object
	//renderer->SetRSToCullNone(true);
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	worldGenerator.DrawShapes();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

void Scene::AddObject(Object* object)
{
	Transform::SetParentChild(root->GetTransform(), object->GetTransform());
}

void Scene::AddObject(Object* object, Object* parent)
{
	Transform::SetParentChild(parent->GetTransform(), object->GetTransform());
}

void Scene::RemoveObject(Object* object)
{
	// remove the the connection and traverse downwards and remove / destroy all objects
}

void Scene::PrintSceneHierarchy(Object* object, size_t level) const
{
	std::string indent = "";

	if (level > 0)
	{
		for (size_t i = 0; i < level; i++)
			indent += "  ";

		indent += "L  ";		
	}
		
	Log::Add(indent + object->GetName());

	if (object->GetTransform().CountChildren() > 0)
	{
		auto children = object->GetTransform().GetChildren();

		for (size_t i = 0; i < children.size(); i++)
		{
			PrintSceneHierarchy(children[i]->GetOwner(), level + 1);
		}
	}
}

