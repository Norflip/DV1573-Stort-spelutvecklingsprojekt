#include "Scene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
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

	worldGenerator.Initialize(renderer->GetDevice());
	worldGenerator.Generate(state, renderer->GetDevice(), root);
	
	worldGenerator.InitalizeGrass(renderer->GetDevice(), renderer->GetContext());

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
	skeletonShader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	skeletonShader.SetInputLayoutStructure(8, skeletonShader.SKELETON_INPUT_LAYOUTd);
	shader.Compile(renderer->GetDevice());
	skeletonShader.Compile(renderer->GetDevice());

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


	testMesh3->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation3));

	//testMesh2->AddComponent<MoveComponent>();

	auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, renderer->GetDevice());
	zwebMaterials[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	sylvanasMat[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	cylinderMat[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	testMesh->AddComponent<MeshComponent>(zwebMeshes[0], zwebMaterials[0]);
	testMesh2->AddComponent<MeshComponent>(sylvanas[0], sylvanasMat[0]);
	testMesh3->AddComponent<MeshComponent>(cylinder[0], cylinderMat[0]);
	
	

	//AddObject(testMesh2);
	//AddObject(testMesh3);

	/* * * * * * * * ** * * * * */
	

	skybox = new Object("Skybox");
	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), skybox);
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
	//AddObject(skyboxClass->GetThisObject());

	

	/* NEW TREE TEST INSTANCED*/
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
	std::vector<Mesh> stylizedTreeModel = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/tree.ZWEB", renderer->GetDevice());
	//0 tree 1 leaves
	std::vector<Material> stylizedTreeMaterial = ZWEBLoader::LoadMaterials("Models/tree.ZWEB", instanceShader, renderer->GetDevice());

	
	stylizedTreeMaterial[0].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
	stylizedTreeMaterial[1].SetSampler(sampler, 0, ShaderBindFlag::PIXEL);

	stylizedTreeMaterial[0].SetShader(instanceShader);
	stylizedTreeMaterial[1].SetShader(alphaInstanceShader);

	worldGenerator.InitializeTrees(stylizedTreeModel, stylizedTreeMaterial, renderer->GetDevice());

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


	




	

	//AddObject(testMesh2);
	//AddObject(testMesh3);
	Object* testMesh4 = new Object("test4");
	testMesh4->AddComponent<NodeWalkerComponent>();
	testMesh4->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation4));
	testMesh4->AddComponent<MeshComponent>(zwebMeshes[0], sylvanasMat[0]);
	
	AddObject(testMesh);
	AddObject(testMesh2);
	AddObject(testMesh3);
	AddObject(testMesh4);

	RigidBodyComp* rigidBody = testMesh2->AddComponent<RigidBodyComp>(4.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING);
	BoxColliderComponent* boxCol = testMesh2->AddComponent<BoxColliderComponent>(dx::XMFLOAT3( 1,1,1 ), dx::XMFLOAT3(0,0,0));

	//rigidBody->m_GenerateCompoundShape();


	physics.RegisterRigidBody(rigidBody);

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

	input.UpdateInputs();
	





	root->Update(deltaTime);
	skyboxClass->GetThisObject()->GetTransform().SetPosition(camera->GetOwner()->GetTransform().GetPosition());
	GameClock::Instance().Update();
	guiManager->UpdateAll();
	//std::cout << "FPS: " << GameClock::Instance().GetFramesPerSecond() << std::endl;
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
				RigidBodyComp* rd = object->AddComponent<RigidBodyComp>(10.0f, FilterGroups::DEFAULT, FilterGroups::EVERYTHING);
		
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

