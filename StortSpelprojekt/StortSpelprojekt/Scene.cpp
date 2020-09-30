#include "Scene.h"

Scene::Scene() : input(Input::Instance())
{
	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
}

Scene::~Scene()
{
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
	camera = cameraObject->AddComponent<CameraComponent>(60.0f);
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

	testMesh->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation));
	
	testMesh2->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation2));
	AddObject(testMesh2, testMesh);


	testMesh3->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation3));
	AddObject(testMesh3, testMesh2);

	testMesh2->AddComponent<MoveComponent>();

	zwebMaterials[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	sylvanasMat[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	cylinderMat[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	
	testMesh->AddComponent<MeshComponent>(zwebMeshes[0], zwebMaterials[0]);
	testMesh2->AddComponent<MeshComponent>(sylvanas[0], sylvanasMat[0]);
	testMesh3->AddComponent<MeshComponent>(cylinder[0], cylinderMat[0]);
	
	AddObject(testMesh);
	//AddObject(testMesh2);
	//AddObject(testMesh3);



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

	
	std::vector<Mesh> treeModels = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/treeEmil.ZWEB", renderer->GetDevice());
	std::vector<Material> treeMaterials = ZWEBLoader::LoadMaterials("Models/treeEmil.ZWEB", instanceShader, renderer->GetDevice());

	treeMaterials[0].SetShader(instanceShader);
	treeMaterials[1].SetShader(alphaInstanceShader);

	size_t nrOfInstances = 10;
	std::vector<Mesh::InstanceData> treeInstances(nrOfInstances);
	std::vector<Mesh::InstanceData> treeBranchInstances(nrOfInstances);
	std::vector<Mesh::InstanceData> treeLeaveInstances(nrOfInstances);

	treeMaterials[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	treeMaterials[1].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	std::vector<unsigned int> r;
	for (int i = 0; i < nrOfInstances; i++)
	{
		r.push_back(rand() % 51 + 1);
	
	}


	for (size_t i = 0; i < nrOfInstances; i++)
	{

		dx::XMStoreFloat4x4(&treeBranchInstances[i].instanceWorld, dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[0].GetT().x, 0 + treeModels[0].GetT().y, (i + 1 * r[i]) + treeModels[0].GetT().z));
		treeBranchInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[0].GetT().x, 0 + treeModels[0].GetT().y, (i + 1 * r[i]) + treeModels[0].GetT().z);

		dx::XMStoreFloat4x4(&treeLeaveInstances[i].instanceWorld, dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[1].GetT().x, 0 + treeModels[1].GetT().y, (i + 1 * r[i]) + treeModels[1].GetT().z));
		treeLeaveInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[1].GetT().x, 0 + treeModels[1].GetT().y, (i + 1 * r[i]) + treeModels[1].GetT().z);

		dx::XMStoreFloat4x4(&treeInstances[i].instanceWorld, dx::XMMatrixScaling(0.5f, 0.5f, 0.5f) * dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[2].GetT().x, 0 + treeModels[2].GetT().y, (i + 1 * r[i]) + treeModels[2].GetT().z));
		treeInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[2].GetT().x, 0 + treeModels[2].GetT().y, (i + 1 * r[i]) + treeModels[2].GetT().z);
	}
	treeModels[0].CreateInstanceBuffer(renderer->GetDevice(), treeBranchInstances);

	treeModels[1].CreateInstanceBuffer(renderer->GetDevice(), treeLeaveInstances);


	treeModels[2].CreateInstanceBuffer(renderer->GetDevice(), treeInstances);

	Object* treeBase = new Object("treeBase");

	Object* treeBranches = new Object("treeBranches");

	Object* leaves = new Object("leaves");


	treeBase->AddComponent<InstancedMeshComponent>(treeModels[2], treeMaterials[0]);
	treeBranches->AddComponent<InstancedMeshComponent>(treeModels[0], treeMaterials[0]);
	leaves->AddComponent<InstancedMeshComponent>(treeModels[1], treeMaterials[1]);


	AddObject(treeBase);

	AddObject(treeBranches);

	AddObject(leaves);
	/*************************INSTANCING****************/


	/* * * * * * * * ** * * * * */

	/*Object* skybox;
	Skybox* skybox = new Skybox(skybox);*/
	/* test skybox */

	Shader skyboxShader;
	skyboxShader.SetPixelShader(L"Shaders/Sky_ps.hlsl");
	skyboxShader.SetVertexShader(L"Shaders/Sky_vs.hlsl");
	skyboxShader.Compile(renderer->GetDevice());

	std::vector<Mesh> zwebSkybox = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/skybox.ZWEB", renderer->GetDevice());
	std::vector<Material> zwebSkyboxMaterials = ZWEBLoader::LoadMaterials("Models/skybox.ZWEB", skyboxShader, renderer->GetDevice());

	testSkybox = new Object("skybox", ObjectFlag::NO_CULL | ObjectFlag::ENABLED);
	testSkybox->AddComponent<MeshComponent>(zwebSkybox, zwebSkyboxMaterials);

	Log::Add("PRINTING SCENE HIERARCHY ----");
	PrintSceneHierarchy(root, 0);
	Log::Add("----");
}

void Scene::Update(const float& deltaTime)
{
	input.UpdateInputs();
	root->Update(deltaTime);
	GameClock::Instance().Update();
}

void Scene::FixedUpdate(const float& fixedDeltaTime)
{
	//Log::Add(std::to_string(fixedDeltaTime));
//	root->FixedUpdate(fixedDeltaTime);
}

void Scene::Render()
{	
	root->Draw(renderer, camera);
	testSkybox->Draw(renderer, camera);
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

