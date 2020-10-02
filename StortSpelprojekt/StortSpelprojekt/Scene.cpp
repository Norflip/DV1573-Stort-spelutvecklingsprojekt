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
}

void Scene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	// TEMP
	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	SaveState state;
	state.seed = 1337;
	state.segment = 0;

	worldGenerator.Initialize(renderer->GetDevice());
	worldGenerator.Generate(state, renderer->GetDevice());


	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(window->GetWidth(), window->GetHeight());
	cameraObject->AddComponent<ControllerComponent>();

	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	AddObject(cameraObject);

	Shader shader;
	Shader skeletonShader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	skeletonShader.SetVertexShader(L"Shaders/Skeleton_vs.hlsl");
	shader.Compile(renderer->GetDevice());

	std::vector<Mesh> zwebMeshes = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/brickSphere.ZWEB", renderer->GetDevice());
	std::vector<Material> zwebMaterials = ZWEBLoader::LoadMaterials("Models/brickSphere.ZWEB", shader, renderer->GetDevice());
	
	std::vector<Mesh> sylvanas = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/sylvanas.ZWEB", renderer->GetDevice());
	std::vector<Material> sylvanasMat = ZWEBLoader::LoadMaterials("Models/sylvanas.ZWEB", shader, renderer->GetDevice());

	std::vector<Mesh> cylinder = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/cylinder.ZWEB", renderer->GetDevice());
	std::vector<Material> cylinderMat = ZWEBLoader::LoadMaterials("Models/cylinder.ZWEB", shader, renderer->GetDevice());

	Object* testMesh = new Object("test");
	Object* testMesh2 = new Object("test2");
	Object* testMesh3 = new Object("test3");
	
	dx::XMFLOAT3 miniTranslation = dx::XMFLOAT3(0, 0, 6);
	dx::XMFLOAT3 miniTranslation2 = dx::XMFLOAT3(2, 2, 2);
	dx::XMFLOAT3 miniTranslation3 = dx::XMFLOAT3(-4, -3, -4);
	dx::XMFLOAT3 miniTranslation4 = dx::XMFLOAT3(0.f, -7.f, 0.f);

	testMesh->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation));
	
	testMesh2->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation2));
	AddObject(testMesh2, testMesh);


	testMesh3->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation3));
	AddObject(testMesh3, testMesh2);

	testMesh2->AddComponent<MoveComponent>();

	auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, renderer->GetDevice());
	zwebMaterials[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	sylvanasMat[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	cylinderMat[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	testMesh->AddComponent<MeshComponent>(zwebMeshes[0], zwebMaterials[0]);
	testMesh2->AddComponent<MeshComponent>(sylvanas[0], sylvanasMat[0]);
	testMesh3->AddComponent<MeshComponent>(cylinder[0], cylinderMat[0]);
	
	AddObject(testMesh);
	//AddObject(testMesh2);
	//AddObject(testMesh3);

	/* * * * * * * * ** * * * * */
	

	skybox = new Object("Skybox");
	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), skybox);
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
	//AddObject(skyboxClass->GetThisObject());

	/*************************INSTANCING*******************/
	Shader instanceShader;
	Shader alphaInstanceShader;

	instanceShader.SetVertexShader(L"Shaders/Instance_vs.hlsl");
	instanceShader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	alphaInstanceShader.SetVertexShader(L"Shaders/Instance_vs.hlsl");
	alphaInstanceShader.SetPixelShader(L"Shaders/Alpha_ps.hlsl");
	alphaInstanceShader.SetInputLayoutStructure(10, alphaInstanceShader.INSTANCE_INPUT_LAYOUTd);
	instanceShader.SetInputLayoutStructure(10, instanceShader.INSTANCE_INPUT_LAYOUTd);
	instanceShader.Compile(renderer->GetDevice());
	alphaInstanceShader.Compile(renderer->GetDevice());
	
	//0 base 1 branch 2 leaves
	std::vector<Mesh> treeModels = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/treeEmil.ZWEB", renderer->GetDevice());
	//0 tree 1 leaves
	std::vector<Material> treeMaterials = ZWEBLoader::LoadMaterials("Models/treeEmil.ZWEB", instanceShader, renderer->GetDevice());

	
	treeMaterials[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	treeMaterials[1].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	treeMaterials[0].SetShader(instanceShader);
	treeMaterials[1].SetShader(alphaInstanceShader);

	size_t nrOfInstances =10;
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
	
	

	
	AddObject(treeBase);

	AddObject(treeBranches);

	AddObject(leaves);
	/*************************INSTANCING****************/

	Object* testMesh4 = new Object("test4");
	testMesh4->AddComponent<NodeWalkerComponent>();
	testMesh4->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation4));
	testMesh4->AddComponent<MeshComponent>(zwebMeshes[0], sylvanasMat[0]);
	AddObject(testMesh4);

	/* * * * * * * * ** * * * * */

	//Log::Add("PRINTING SCENE HIERARCHY ----");
	//PrintSceneHierarchy(root, 0);
	/*Log::Add("----");*/
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

