#include "GameScene.h"
#include "GUISprite.h"
#include "GUIFont.h"

GameScene::GameScene(ResourceManager* manager) : Scene(manager)
{
}

GameScene::~GameScene()
{
}

void GameScene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	Physics& physics = Physics::Instance();
	physics.Initialize({ 0, -10.0f, 0 });

	SaveState state;
	state.seed = 1337;
	state.segment = 0;

	worldGenerator.Initialize(renderer->GetDevice(), resourceManager->GetShaderResource("terrainShader"), resourceManager->GetShaderResource("grassShader"));
	worldGenerator.Generate(state, renderer->GetDevice(), root);
	worldGenerator.InitalizeGrass(renderer->GetDevice(), renderer->GetContext());

	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(window->GetWidth(), window->GetHeight());
	cameraObject->AddComponent<ControllerComponent>();


	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	AddObject(cameraObject);

}

void GameScene::InitializeObjects()
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

	Shader* lightShader = resourceManager->GetShaderResource("defaultShader");

	//TEST POINT LIGHTS____________________________________________________________________________________________________________________
	Object* testPointLight = new Object("testPointLight");
	dx::XMFLOAT3 lightTranslation = dx::XMFLOAT3(2.0f, 0.0f, 3.0f);
	testPointLight->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation));
	AddObject(testPointLight);
	testPointLight->AddComponent<PointLightComponent>();
	testPointLight->GetComponent<PointLightComponent>()->SetColor({ 1.f, 0.f, 0.f, 1.f });

	Object* testPointLight2 = new Object("testPointLight2");
	dx::XMFLOAT3 lightTranslation2 = dx::XMFLOAT3(0.0f, 2.0f, 3.0f);
	testPointLight2->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation2));
	AddObject(testPointLight2);
	testPointLight2->AddComponent<PointLightComponent>();
	testPointLight2->GetComponent<PointLightComponent>()->SetColor({ 0.f, 1.f, 0.f, 1.f });

	Object* testPointLight3 = new Object("testPointLight3");
	dx::XMFLOAT3 lightTranslation3 = dx::XMFLOAT3(-2.0f, 0.0f, 3.0f);
	testPointLight3->GetTransform().SetPosition(dx::XMLoadFloat3(&lightTranslation3));
	AddObject(testPointLight3);
	testPointLight3->AddComponent<PointLightComponent>();
	testPointLight3->GetComponent<PointLightComponent>()->SetColor({ 0.f, 0.f, 1.f, 1.f });
	//_____________________________________________________________________________________________________________________________________

	stylizedTreeMaterial[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	stylizedTreeMaterial[1].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	stylizedTreeMaterial[0].SetShader(instanceShader);
	stylizedTreeMaterial[1].SetShader(alphaInstanceShader);

	worldGenerator.InitializeTrees(stylizedTreeModel, stylizedTreeMaterial, renderer->GetDevice());
	//
	Shader* skeletonShader = resourceManager->GetShaderResource("skeletonShader");

	std::vector<Mesh> skeletonMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/baseMonster.ZWEB", renderer->GetDevice());
	std::vector<Material> skeletonMat = ZWEBLoader::LoadMaterials("Models/baseMonster.ZWEB", skeletonShader, renderer->GetDevice());

	SkeletonAni skeletonbaseMonsterIdle = ZWEBLoader::LoadSkeletonOnly("Models/baseMonsterIdle.ZWEB", skeletonMesh[0].GetBoneIDS());

	Object* baseMonsterObject = new Object("baseMonster");

	SkeletonMeshComponent* baseMonsterComp = baseMonsterObject->AddComponent<SkeletonMeshComponent>(skeletonMesh[0], skeletonMat[0]);

	baseMonsterComp->SetAnimationTrack(skeletonbaseMonsterIdle, StateMachine::IDLE);

	baseMonsterObject->GetTransform().SetScale({ 0.125f, 0.125f, 0.125f });
	baseMonsterObject->GetTransform().SetPosition({ 0.0f, 2.5f, 0.0f });
	AddObject(baseMonsterObject);

	/* NEW TREE TEST INSTANCED*/

	clock.Update();
	clock.Start();
	clock.Update();

	/* * * * * * * * ** * * * * */
	//Log::Add("PRINTING SCENE HIERARCHY ----");
	//PrintSceneHierarchy(root, 0);
	/*Log::Add("----");*/
}

void GameScene::InitializeGUI()
{
	spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* normalSprite = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::BottomLeft, ClickFunction::Clickable);
	GUISprite* buttonSprite = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::BottomRight, ClickFunction::Clickable);
	GUISprite* normalSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::TopLeft, ClickFunction::Clickable);
	GUISprite* buttonSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::TopRight, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "test", 300, 300);
	normalSprite->SetActive();

	guiManager = new GUIManager(renderer);
	renderer->SetGUIManager(guiManager);
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(normalSprite, "normalSprite");
	guiManager->AddGUIObject(buttonSprite, "buttonSprite");
	guiManager->AddGUIObject(normalSprite2, "normalSprite2");
	guiManager->AddGUIObject(buttonSprite2, "buttonSprite2");
	guiManager->GetGUIObject("normalSprite")->SetPosition(100, 100);
}

void GameScene::OnActivate()
{
	InitializeGUI();
	InitializeObjects();
}

void GameScene::OnDeactivate()
{
}

void GameScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);

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
				RigidBodyComponent* rd = object->AddComponent<RigidBodyComponent>(10.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING);

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

	renderer->RenderFrame();
}

