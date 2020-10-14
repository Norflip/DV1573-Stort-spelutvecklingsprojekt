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

	resourceManager = new ResourceManager;
	resourceManager->InitializeResources(renderer->GetDevice());

	// TEMP
	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();
	
	Physics& physics = Physics::Instance();
	physics.Initialize({ 0, -10.0f, 0 });

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

	//for (size_t i = 0; i < nrOfInstances; i++)
	//{
	//	dx::XMStoreFloat4x4(&treeBranchInstances[i].instanceWorld, dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[1].GetT().x, 0 + treeModels[0].GetT().y, (i + 1 * r[i]) + treeModels[1].GetT().z));
	//	treeBranchInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[1].GetT().x, 0 + treeModels[1].GetT().y, (i + 1 * r[i]) + treeModels[1].GetT().z);
	//}

	//size_t nrOfInstancedStyTrees =5;
	//std::vector<Mesh::InstanceData> styTreesInstanced(nrOfInstancedStyTrees);
	//std::vector<Mesh::InstanceData> styLeavesInstanced(nrOfInstancedStyTrees);

	//
	//std::vector<unsigned int> randNr;
	//for (size_t i = 0; i < nrOfInstancedStyTrees; i++)
	//{
	//	randNr.push_back(rand() % 5 + 1);
	//}

	/*for (size_t i = 0; i < nrOfInstancedStyTrees; i++)
	{		
		dx::XMStoreFloat4x4(&styLeavesInstanced[i].instanceWorld, dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 6 * randNr[i]) + stylizedTreeModel[1].GetT().x, 0 + stylizedTreeModel[1].GetT().y, (i + 1 * randNr[i]) + stylizedTreeModel[1].GetT().z));
		styLeavesInstanced[i].instancePosition = dx::XMFLOAT3((i + 1 * randNr[i]) + stylizedTreeModel[1].GetT().x, 0 + stylizedTreeModel[1].GetT().y, (i + 1 * randNr[i]) + stylizedTreeModel[1].GetT().z);

		dx::XMStoreFloat4x4(&styTreesInstanced[i].instanceWorld,dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 6 * randNr[i]) + stylizedTreeModel[0].GetT().x, 0 + stylizedTreeModel[0].GetT().y, (i + 1 * randNr[i]) + stylizedTreeModel[0].GetT().z));
		styTreesInstanced[i].instancePosition = dx::XMFLOAT3((i + 1 * randNr[i]) + stylizedTreeModel[0].GetT().x, 0 + stylizedTreeModel[0].GetT().y, (i + 1 * randNr[i]) + stylizedTreeModel[0].GetT().z);
	}

	stylizedTreeModel[0].CreateInstanceBuffer(renderer->GetDevice(), styTreesInstanced);
	stylizedTreeModel[1].CreateInstanceBuffer(renderer->GetDevice(), styLeavesInstanced);
		
	stylizedTreeModel[0].SetInstanceNr(nrOfInstancedStyTrees);
	stylizedTreeModel[1].SetInstanceNr(nrOfInstancedStyTrees);

	Object* styTreeBase = new Object("treeBase");
	Object* styLeavesBase = new Object("leaves");

	stylizedTreeMaterial[1].SetTransparent(true);
	styTreeBase->AddComponent<InstancedMeshComponent>(stylizedTreeModel[0], stylizedTreeMaterial[0]);
	styLeavesBase->AddComponent<InstancedMeshComponent>(stylizedTreeModel[1], stylizedTreeMaterial[1]);

	styLeavesBase->AddFlag(ObjectFlag::NO_CULL);
	
	AddObject(styTreeBase);
	AddObject(styLeavesBase);*/

	/* NEW TREE TEST INSTANCED*/


	




	

	clock.Update();
	clock.Start();
	clock.Update();

	/* * * * * * * * ** * * * * */
	//Log::Add("PRINTING SCENE HIERARCHY ----");
	//PrintSceneHierarchy(root, 0);
	/*Log::Add("----");*/
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

	guiManager->UpdateAll();
	
	renderer->UpdateTime((float)clock.GetSeconds());
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

	renderer->RenderFrame();
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

