#include "Scene.h"

Scene::Scene() : input(Input::Instance())
{
	

}

Scene::~Scene()
{
	for (int i = 0; i < objects.size(); i++)
	{
		if (objects[i])
		{
			delete objects[i];
		}
	}
	for (int i = 0; i < InstancedObjects.size(); i++)
	{
		if (InstancedObjects[i])
		{
			delete InstancedObjects[i];
		}
	}

	for (int i = 0; i < alphaInstancedObjects.size(); i++)
	{
		if (alphaInstancedObjects[i])
		{
			delete alphaInstancedObjects[i];
		}
	}

	/*if (move)
	{
		delete move;
	}*/
	if (renderer)
	{
		delete renderer;
	}
	
	if (quad)
	{
		delete quad;
	}
	if (screenquadTex)
	{
		delete screenquadTex;
	}


	




	
}

void Scene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	// TEMP
	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();
	
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f);
	camera->Resize(window->GetWidth(), window->GetHeight());
	move = cameraObject->AddComponent<ControllerComponent>();
	cameraObject->AddFlag(ObjectFlag::NO_CULL);
	objects.push_back(cameraObject);
	
	Shader shader;
	Shader skeletonShader;
	Shader instanceShader;
	Shader alphaInstanceShader;
	
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	skeletonShader.SetVertexShader(L"Shaders/Skeleton_vs.hlsl");
	instanceShader.SetVertexShader(L"Shaders/Instance_vs.hlsl");
	instanceShader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	alphaInstanceShader.SetVertexShader(L"Shaders/Instance_vs.hlsl");
	alphaInstanceShader.SetPixelShader(L"Shaders/Alpha_ps.hlsl");
	shader.SetDefaultInputLayout();
	shader.Compile(renderer->GetDevice());
	instanceShader.SetInstanceLayout();
	instanceShader.Compile(renderer->GetDevice());
	alphaInstanceShader.SetInstanceLayout();
	alphaInstanceShader.Compile(renderer->GetDevice());

	

	
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
	testMesh->AddFlag(ObjectFlag::NO_CULL);
	testMesh2->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation2));
	Transform::SetParentChild(testMesh->GetTransform(), testMesh2->GetTransform());

	testMesh3->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation3));
	Transform::SetParentChild(testMesh2->GetTransform(), testMesh3->GetTransform());

	
	sylvanasMat[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	cylinderMat[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	
	testMesh2->AddComponent<MeshComponent>(sylvanas[0], sylvanasMat[0]);
	testMesh2->GetComponent<MeshComponent>()->GetBoundingBoxes().CalcAABB();
	testMesh2->AddFlag(ObjectFlag::NO_CULL);
	testMesh3->AddComponent<MeshComponent>(cylinder[0], cylinderMat[0]);
	testMesh3->GetComponent<MeshComponent>()->GetBoundingBoxes().CalcAABB();
	testMesh3->AddFlag(ObjectFlag::NO_CULL);
	objects.push_back(testMesh);
	objects.push_back(testMesh2);
	objects.push_back(testMesh3);




	//Instanced Trees test
	//0 branches, 1 leaves
	
	std::vector<Mesh> treeModels = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/treeEmil.ZWEB", renderer->GetDevice());
	std::vector<Material> treeMaterials = ZWEBLoader::LoadMaterials("Models/treeEmil.ZWEB", instanceShader, renderer->GetDevice());

	treeMaterials[1].SetShader(alphaInstanceShader);

	size_t nrOfInstances = 100;
	std::vector<Mesh::InstanceData> treeInstances(nrOfInstances);
	
	treeMaterials[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	treeMaterials[1].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	
	std::vector<unsigned int> r;
	for (int i = 0; i < nrOfInstances; i++)
	{
		r.push_back(rand() % 51 + 1);
		//r.push_back(1);
	}
	

	for (size_t i = 0; i < nrOfInstances; i++)
	{
		
		dx::XMStoreFloat4x4(&treeInstances[i].instanceWorld,dx::XMMatrixTranslation((i + 1 *r[i] ) + treeModels[0].GetT().x, 0 + treeModels[0].GetT().y, 10 + treeModels[0].GetT().z));
		treeInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[0].GetT().x, 0 + treeModels[0].GetT().y, 10 + treeModels[0].GetT().z);

	}
	treeModels[0].CreateInstanceBuffer(renderer->GetDevice(), treeInstances);
	for (size_t i = 0; i < nrOfInstances; i++)
	{

		dx::XMStoreFloat4x4(&treeInstances[i].instanceWorld, dx::XMMatrixTranslation((i + 1 * r[i])+ treeModels[1].GetT().x, 0 + treeModels[1].GetT().y, 10+ treeModels[1].GetT().z));
		treeInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[1].GetT().x, 0 + treeModels[1].GetT().y, 10 + treeModels[1].GetT().z);

	}
	
	treeModels[1].CreateInstanceBuffer(renderer->GetDevice(), treeInstances);

	for (size_t i = 0; i < nrOfInstances; i++)
	{

		dx::XMStoreFloat4x4(&treeInstances[i].instanceWorld, dx::XMMatrixTranslation((i + 1 * r[i]) + treeModels[2].GetT().x, 0 + treeModels[2].GetT().y, 10 + treeModels[2].GetT().z));
		treeInstances[i].instancePosition = dx::XMFLOAT3((i + 1 * r[i]) + treeModels[2].GetT().x, 0 + treeModels[2].GetT().y, 10 + treeModels[2].GetT().z);

	}

	treeModels[2].CreateInstanceBuffer(renderer->GetDevice(), treeInstances);


	Object* treeBase = new Object("treeBase");

	Object* treeBranches = new Object("treeBranches");

	treeModels[0].SetInstanceNr(nrOfInstances);
	treeModels[1].SetInstanceNr(nrOfInstances);
	treeModels[2].SetInstanceNr(nrOfInstances);


	treeBase->AddComponent<MeshComponent>(treeModels[2], treeMaterials[0]);
	treeBase->GetComponent<MeshComponent>()->GetBoundingBoxes().CalcAABB();
	treeBranches->AddComponent<MeshComponent>(treeModels[0], treeMaterials[0]);
	treeBranches->GetComponent<MeshComponent>()->GetBoundingBoxes().CalcAABB();
	
	

	InstancedObjects.push_back(treeBase);
	InstancedObjects.push_back(treeBranches);
	
	Object* leaves = new Object("leaves");

	leaves->AddComponent<MeshComponent>(treeModels[1], treeMaterials[1]);

	

	alphaInstancedObjects.push_back(leaves);
	

	/* * * * * * * * ** * * * * */
	/* Render to texture test */	
	screenquadTex = new Texture;
	renderer->RenderToTexture(screenquadTex, renderer->GetDevice(), window->GetWidth(), window->GetHeight());

	/* Screenquad shader */
	Shader screenquadShader;
	screenquadShader.SetPixelShader(L"Shaders/ScreenQuad_ps.hlsl");
	screenquadShader.SetVertexShader(L"Shaders/ScreenQuad_vs.hlsl");
	screenquadShader.SetDefaultInputLayout();
	screenquadShader.Compile(renderer->GetDevice());

	/* Screenquad mat */
	screenquadmat = Material(screenquadShader);

	/* Screenquad mesh */
	Mesh screenquadMesh = Mesh::CreateScreenQuad(renderer->GetDevice());
	screenquadmat.SetTexture(*screenquadTex, TEXTURE_DIFFUSE_SLOT, ShaderBindFlag::PIXEL);
	screenquadmat.SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	/* Screenquad object */
	quad = new Object("Screenquad");
	quad->AddComponent<MeshComponent>(screenquadMesh, screenquadmat);	

	/* * * * * * * * ** * * * * */
	//PrintSceneHierarchy();

}

void Scene::Update(const float& deltaTime)
{
	std::vector<Object*> toRemove;
	input.UpdateInputs();


	for (auto i = objects.begin(); i < objects.end(); i++)
	{
		Object* obj = (*i);

		if (obj->HasFlag(ObjectFlag::ENABLED))
			obj->Update(deltaTime);

		if (obj->HasFlag(ObjectFlag::REMOVED))
			toRemove.push_back(obj);
	}

}

void Scene::FixedUpdate(const float& fixedDeltaTime)
{
	//Log::Add(std::to_string(fixedDeltaTime));
}

void Scene::Render()
{	
	renderer->BeginFrame();
	camera->GetFrustumPlanes(extractedPlanes);
	renderer->SetBlendState(false);

	for (auto i = objects.begin(); i < objects.end(); i++)
	{
		Object* obj = (*i);

		if (obj->HasFlag(ObjectFlag::NO_CULL))
		{

			obj->Draw(renderer, camera, DrawType::STANDARD);
		}
		else
		{
			dx::XMStoreFloat3(&tempObjectPos, obj->GetTransform().GetPosition());//If object is rotating around something else, then maybe decompose world matrix.

			if (!camera->CullAgainstAABB(extractedPlanes, obj->GetComponent<MeshComponent>()->GetBoundingBoxes().GetAABB(), tempObjectPos))
			{
				obj->Draw(renderer, camera, DrawType::STANDARD);
			}
			else
			{
				//std::cout << "THIS IS NOT VISIBLE";
			}


		}

	}
	for (auto i = InstancedObjects.begin(); i < InstancedObjects.end(); i++)
	{
		Object* obj = (*i);

		
		
		
		

		if (obj->HasFlag(ObjectFlag::NO_CULL))
		{

			obj->Draw(renderer, camera, DrawType::INSTANCED);
		}
		else
		{
			nrOfInstancesToDraw = 0;
			BoundingBoxes& tempBB = obj->GetComponent<MeshComponent>()->GetBoundingBoxes();
			Mesh& tempMesh = obj->GetComponent<MeshComponent>()->GetMesh();
			tempMesh.SetInstanceNr(nrOfInstancesToDraw);
			renderer->GetContext()->Map(tempMesh.instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData); //the instance buffer is dynmaic so you can update it.

			Mesh::InstanceData* dataView = reinterpret_cast<Mesh::InstanceData*>(mappedData.pData); //a safe cast

			 //get mesh innan for loopen template kan ta tid.

			for (UINT instance = 0; instance < (UINT)tempMesh.instanceData.size(); instance++) //cull all the instances
			{
				tempObjectPos = tempMesh.instanceData[instance].instancePosition;
				if (!camera->CullAgainstAABB(extractedPlanes, tempBB.GetAABB(), tempObjectPos))
				{
					dataView[nrOfInstancesToDraw++] = tempMesh.instanceData[instance];
					
				}

			}
			renderer->GetContext()->Unmap(tempMesh.instanceBuffer, 0);
			tempMesh.SetInstanceNr(nrOfInstancesToDraw);
			if (nrOfInstancesToDraw > 0)
			{
				obj->Draw(renderer, camera, DrawType::INSTANCED);
				/*std::cout << "\n\n";
				std::cout << "OBJECTS NOT CULLED: ";
				std::cout << nrOfInstancesToDraw;
				std::cout << "\n\n";*/
			}
			
		}

	}
	renderer->SetBlendState(true);
	for (auto i = alphaInstancedObjects.begin(); i < alphaInstancedObjects.end(); i++)
	{
		Object* obj = (*i);

		obj->Draw(renderer, camera, DrawType::INSTANCEDALPHA);

	}

	renderer->EndFrame();
}

void Scene::RenderSceneToTexture()
{
	renderer->Unbind();
	
	renderer->SetRenderTarget(renderer->GetContext(), screenquadTex->GetRtv());
	renderer->ClearRenderTarget(renderer->GetContext(), screenquadTex->GetRtv(), dx::XMFLOAT4(0.3f, 0.1f, 0.2f, 1.0f));
	
	


	//renderer->Unbind();	// needed?

	renderer->SetBackbufferRenderTarget();
}

void Scene::PrintSceneHierarchy() const
{
	Log::Add("PRINTING SCENE HIERARCHY ----");

	for (auto i = objects.cbegin(); i < objects.cend(); i++)
		PrintSceneHierarchy(*i, 0);

	Log::Add("----");
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

