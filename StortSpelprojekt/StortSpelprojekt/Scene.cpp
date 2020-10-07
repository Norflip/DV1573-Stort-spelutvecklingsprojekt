#include "Scene.h"

Scene::Scene() : input(Input::Instance())
{
	skyboxClass = nullptr;
	skybox = nullptr;
	renderer = nullptr;
	camera = nullptr;

	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
}

Scene::~Scene()
{	
	delete skybox;
	skybox = nullptr;

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

	SaveState state;
	state.seed = 1337;
	state.segment = 0;

	worldGenerator.Initialize(renderer->GetDevice(), resourceManager->GetShaderResource("terrainShader"));
	worldGenerator.Generate(state, renderer->GetDevice());

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
	std::vector<Mesh> treeModels = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/treeEmil.ZWEB", renderer->GetDevice());
	//0 tree 1 leaves
	std::vector<Material> treeMaterials = ZWEBLoader::LoadMaterials("Models/treeEmil.ZWEB", instanceShader, renderer->GetDevice());

	treeMaterials[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	treeMaterials[1].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	treeMaterials[0].SetShader(instanceShader);
	treeMaterials[1].SetShader(alphaInstanceShader);

	size_t nrOfInstances = 10;
	std::vector<Mesh::InstanceData> treeInstances(nrOfInstances);
	std::vector<Mesh::InstanceData> treeBranchInstances(nrOfInstances);
	std::vector<Mesh::InstanceData> treeLeaveInstances(nrOfInstances);

	std::vector<unsigned int> r;
	for (size_t i = 0; i < nrOfInstances; i++)
	{
		r.push_back(rand() % 10 + 1);
	}

	for (size_t i = 0; i < nrOfInstances; i++)
	{
		dx::XMStoreFloat4x4(&treeBranchInstances[i].instanceWorld, dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[1].GetT().x, 0 + treeModels[0].GetT().y, (i + 1 * r[i]) + treeModels[1].GetT().z));
		treeBranchInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[1].GetT().x, 0 + treeModels[1].GetT().y, (i + 1 * r[i]) + treeModels[1].GetT().z);

		dx::XMStoreFloat4x4(&treeLeaveInstances[i].instanceWorld, dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[2].GetT().x, 0 + treeModels[2].GetT().y, (i + 1 * r[i]) + treeModels[2].GetT().z));
		treeLeaveInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[2].GetT().x, 0 + treeModels[2].GetT().y, (i + 1 * r[i]) + treeModels[2].GetT().z);

		dx::XMStoreFloat4x4(&treeInstances[i].instanceWorld,dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[0].GetT().x, 0 + treeModels[2].GetT().y, (i + 1 * r[i]) + treeModels[0].GetT().z));
		treeInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[0].GetT().x, 0 + treeModels[0].GetT().y, (i + 1 * r[i]) + treeModels[0].GetT().z);
	}

	treeModels[0].CreateInstanceBuffer(renderer->GetDevice(), treeInstances );
	treeModels[1].CreateInstanceBuffer(renderer->GetDevice(), treeBranchInstances );
	treeModels[2].CreateInstanceBuffer(renderer->GetDevice(), treeLeaveInstances);

	treeModels[0].SetInstanceNr(nrOfInstances);
	treeModels[1].SetInstanceNr(nrOfInstances);
	treeModels[2].SetInstanceNr(nrOfInstances);

	Object* treeBase = new Object("treeBase");
	Object* treeBranches = new Object("treeBranches");
	Object* leaves = new Object("leaves");

	treeMaterials[1].SetTransparent(true);

	treeBase->AddComponent<InstancedMeshComponent>(treeModels[0], treeMaterials[0]);
	treeBranches->AddComponent<InstancedMeshComponent>(treeModels[1], treeMaterials[0]);
	leaves->AddComponent<InstancedMeshComponent>(treeModels[2], treeMaterials[1]);

	leaves->AddFlag(ObjectFlag::NO_CULL);

	AddObject(treeBranches, treeBase);
	AddObject(leaves, treeBranches);
	AddObject(treeBase);

	///*************************INSTANCING****************/
}

void Scene::Update(const float& deltaTime)
{
	dx::XMFLOAT3 positionA = { 0,0,2 };
	dx::XMFLOAT3 positionB = { 0, 2,-5};

	DShape::DrawBox(positionA, { 2,2,2 }, { 0, 1, 1 });
	DShape::DrawWireBox(positionB, { 4,4,4 }, { 1,0,0 });

	DShape::DrawSphere({ -4,0,0 }, 1.0f, { 0, 0, 1 });
	DShape::DrawWireSphere({ -4,0,5 }, 1.0f, { 0,1,0 });

	DShape::DrawLine(positionA, positionB, { 1,1,0 });

	input.UpdateInputs();
	root->Update(deltaTime);
	skyboxClass->GetThisObject()->GetTransform().SetPosition(camera->GetOwner()->GetTransform().GetPosition());
	GameClock::Instance().Update();
	//std::cout << "FPS: " << GameClock::Instance().GetFramesPerSecond() << std::endl;
	
	// Press P to recompile shaders
	if (KEY_PRESSED(P))
	{
		resourceManager->CompileShaders(renderer->GetDevice());
	}
}

void Scene::FixedUpdate(const float& fixedDeltaTime)
{
	//Log::Add(std::to_string(fixedDeltaTime));
//	root->FixedUpdate(fixedDeltaTime);
}

void Scene::Render()
{	
	// skybox draw object
	renderer->SetRSToCullNone(true);
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);

	worldGenerator.Draw(renderer, camera);

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

