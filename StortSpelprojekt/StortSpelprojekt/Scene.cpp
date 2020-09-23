#include "Scene.h"

Scene::Scene() : input(Input::Instance())
{
	

}

Scene::~Scene()
{
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
	objects.push_back(cameraObject);
	
	Shader shader;
	Shader skeletonShader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	skeletonShader.SetVertexShader(L"Shaders/Skeleton_vs.hlsl");
	
	shader.Compile(renderer->GetDevice());

	std::vector<Mesh> zwebMeshes = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "Models/brickSphere.ZWEB", renderer->GetDevice());
	std::vector<Material> zwebMaterials = ZWEBLoader::LoadMaterials("Models/brickSphere.ZWEB", shader, renderer->GetDevice());

	BoundingBoxComponent cubeBB;

	cubeBB.CalcAABB(zwebMeshes[0]);

	Object* testMesh = new Object("test");

	dx::XMFLOAT3 miniTranslation = dx::XMFLOAT3(0, 0, 3);

	testMesh->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation));

	testMesh->AddComponent<BoundingBoxComponent>(cubeBB);

	testMesh->AddFlag(ObjectFlag::BOUNDINGBOX);
	zwebMaterials[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	testMesh->AddComponent<MeshComponent>(zwebMeshes[0], zwebMaterials[0]);
	objects.push_back(testMesh);


	Mesh mesh = ShittyOBJLoader::Load("Models/Cube.obj", renderer->GetDevice());

	Material material = Material(shader);

	/* Loading a texture */
	Texture diffuseTexture;
	diffuseTexture.LoadTexture(renderer->GetDevice(), L"Textures/Gorilla.png");
	Texture randomNormal;
	randomNormal.LoadTexture(renderer->GetDevice(), L"Textures/RandomNormal.png");

	/* Setting texture to correct slot in material*/
	material.SetTexture(diffuseTexture, TEXTURE_DIFFUSE_SLOT, ShaderBindFlag::PIXEL);
	material.SetTexture(randomNormal, TEXTURE_NORMAL_SLOT, ShaderBindFlag::PIXEL);
	material.SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	Object* tmp_obj = new Object("cube1");
	tmp_obj->GetTransform().SetPosition({ 0, 0, 10 });
	
	
	
	tmp_obj->AddFlag(ObjectFlag::ENABLED | ObjectFlag::RENDER);
	tmp_obj->AddComponent<MeshComponent>(mesh, material);
	objects.push_back(tmp_obj);

	Object* tmp_obj2 = new Object("cube2");
	tmp_obj2->GetTransform().SetPosition({ 0, 0, 4 });

	tmp_obj2->AddFlag(ObjectFlag::ENABLED | ObjectFlag::RENDER);
	tmp_obj2->AddComponent<MeshComponent>(mesh, material);
	tmp_obj2->AddComponent<MoveComponent>();
	
	Transform::SetParentChild(tmp_obj->GetTransform(), tmp_obj2->GetTransform());
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
	Transform::SetParentChild(testMesh->GetTransform(), testMesh2->GetTransform());

	testMesh3->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation3));
	Transform::SetParentChild(testMesh2->GetTransform(), testMesh3->GetTransform());

	zwebMaterials[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	sylvanasMat[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
	cylinderMat[0].SetSamplerState(renderer->GetDevice(), D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);

	testMesh->AddComponent<MeshComponent>(zwebMeshes[0], zwebMaterials[0]);
	testMesh2->AddComponent<MeshComponent>(sylvanas[0], sylvanasMat[0]);
	testMesh3->AddComponent<MeshComponent>(cylinder[0], cylinderMat[0]);

	objects.push_back(testMesh);
	objects.push_back(testMesh2);
	objects.push_back(testMesh3);


	/* * * * * * * * ** * * * * */
	/* Render to texture test */	
	screenquadTex = new Texture;
	renderer->RenderToTexture(screenquadTex, renderer->GetDevice(), window->GetWidth(), window->GetHeight());

	/* Screenquad shader */
	Shader screenquadShader;
	screenquadShader.SetPixelShader(L"Shaders/ScreenQuad_ps.hlsl");
	screenquadShader.SetVertexShader(L"Shaders/ScreenQuad_vs.hlsl");
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
	RenderSceneToTexture();
	//for (auto i = objects.begin(); i < objects.end(); i++)
	//{
	//	Object* obj = (*i);
	//	//if (obj->HasFlag(ObjectFlag::ENABLED | ObjectFlag::RENDER))
	//	obj->Draw(renderer, camera);
	//}

	/* Render screenquad with rendered scene-texture */
	quad->Draw(renderer, camera);
	renderer->EndFrame();
}

void Scene::RenderSceneToTexture()
{
	renderer->Unbind();

	renderer->SetRenderTarget(renderer->GetContext(), screenquadTex->GetRtv());
	renderer->ClearRenderTarget(renderer->GetContext(), screenquadTex->GetRtv(), dx::XMFLOAT4(0, 1, 0, 1));

	for (auto i = objects.begin(); i < objects.end(); i++)
	{
		GetFrustumPlanes(extractedPlanes);
		Object* obj = (*i);
		if (obj->HasFlag(ObjectFlag::BOUNDINGBOX))
		{
			for (int aabb = 0; aabb < obj->GetComponent<BoundingBoxComponent>()->GetNrOfAABBs(); aabb++)
			{
				if (!CullAgainstAABB(extractedPlanes, obj->GetComponent<BoundingBoxComponent>()->GetAABBs()[aabb]))
				{
					obj->Draw(renderer, camera);
				}
				else
				{
					//std::cout << "THIS IS NOT VISIBLE";
				}
				
			}
			
		}
		else
		{
			obj->Draw(renderer, camera);
		}
			
	}

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

void Scene::GetFrustumPlanes(std::vector<dx::XMFLOAT4>& planes)
{

	// x, y, z, and w represent A, B, C and D in the plane equation
	// where ABC are the xyz of the planes normal, and D is the plane constant
	planes.resize(6);
	//r means row
	
	

	

	// Left Frustum Plane
   // Add first column of the matrix to the fourth column
	planes[0].x = camera->GetVIewAndProjectionMatrix().r[0].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[0].m128_f32[0];
	planes[0].y = camera->GetVIewAndProjectionMatrix().r[1].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[1].m128_f32[0];
	planes[0].z = camera->GetVIewAndProjectionMatrix().r[2].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[2].m128_f32[0];
	planes[0].w = camera->GetVIewAndProjectionMatrix().r[3].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[3].m128_f32[0];

	// Right Frustum Plane
	// Subtract first column of matrix from the fourth column

	planes[1].x = camera->GetVIewAndProjectionMatrix().r[0].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[0].m128_f32[0];
	planes[1].y = camera->GetVIewAndProjectionMatrix().r[1].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[1].m128_f32[0];
	planes[1].z = camera->GetVIewAndProjectionMatrix().r[2].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[2].m128_f32[0];
	planes[1].w = camera->GetVIewAndProjectionMatrix().r[3].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[3].m128_f32[0];

	// Top Frustum Plane
	// Subtract second column of matrix from the fourth column

	planes[2].x = camera->GetVIewAndProjectionMatrix().r[0].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[0].m128_f32[1];
	planes[2].y = camera->GetVIewAndProjectionMatrix().r[1].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[1].m128_f32[1];
	planes[2].z = camera->GetVIewAndProjectionMatrix().r[2].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[2].m128_f32[1];
	planes[2].w = camera->GetVIewAndProjectionMatrix().r[3].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[3].m128_f32[1];

	// Bottom Frustum Plane
	// Add second column of the matrix to the fourth column

	planes[3].x = camera->GetVIewAndProjectionMatrix().r[0].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[0].m128_f32[1];
	planes[3].y = camera->GetVIewAndProjectionMatrix().r[1].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[1].m128_f32[1];
	planes[3].z = camera->GetVIewAndProjectionMatrix().r[2].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[2].m128_f32[1];
	planes[3].w = camera->GetVIewAndProjectionMatrix().r[3].m128_f32[3] + camera->GetVIewAndProjectionMatrix().r[3].m128_f32[1];


	// Near Frustum Plane
	// We could add the third column to the fourth column to get the near plane,
	// but we don't have to do this because the third column IS the near plane


	planes[4].x = camera->GetVIewAndProjectionMatrix().r[0].m128_f32[2];
	planes[4].y = camera->GetVIewAndProjectionMatrix().r[1].m128_f32[2];
	planes[4].z = camera->GetVIewAndProjectionMatrix().r[2].m128_f32[2];
	planes[4].w = camera->GetVIewAndProjectionMatrix().r[3].m128_f32[2];

	// Far Frustum Plane
	// Subtract third column of matrix from the fourth column

	planes[5].x = camera->GetVIewAndProjectionMatrix().r[0].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[0].m128_f32[2];
	planes[5].y = camera->GetVIewAndProjectionMatrix().r[1].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[1].m128_f32[2];
	planes[5].z = camera->GetVIewAndProjectionMatrix().r[2].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[2].m128_f32[2];
	planes[5].w = camera->GetVIewAndProjectionMatrix().r[3].m128_f32[3] - camera->GetVIewAndProjectionMatrix().r[3].m128_f32[2];


	// Normalize plane normals (A, B and C (xyz))
   // Also take note that planes face inward

	for (int i = 0; i < 6; ++i)
	{
		float length = sqrt((planes[i].x * planes[i].x) + (planes[i].y * planes[i].y) + (planes[i].z * planes[i].z));
		planes[i].x /= length;
		planes[i].y /= length;
		planes[i].z /= length;
		planes[i].w /= length;
	}




}

bool Scene::CullAgainstAABB(std::vector<dx::XMFLOAT4>& planes, const AABB& aabb)
{
	for (unsigned int plane = 0; plane < 6; ++plane)
	{
		DirectX::XMVECTOR planeNormal = DirectX::XMVectorSet(planes[plane].x, planes[plane].y, planes[plane].z, 0.0f);
		float planeConstant = planes[plane].w;

		DirectX::XMFLOAT3 diagonal;

		if (planes[plane].x >= 0.0f)
		{
			diagonal.x = aabb.max.x - aabb.min.x;
		}
		else
		{
			diagonal.x = aabb.min.x - aabb.max.x;
		}
		if (planes[plane].y >= 0.0f)
		{
			diagonal.y = aabb.max.y - aabb.min.y;
		}
		else
		{
			diagonal.y = aabb.min.y - aabb.max.y;
		}
		if (planes[plane].z >= 0.0f)
		{
			diagonal.z = aabb.max.z - aabb.min.z;
		}
		else
		{
			diagonal.z = aabb.min.z - aabb.max.z;
		}

		

		if (DirectX::XMVectorGetX(DirectX::XMVector3Dot(planeNormal, DirectX::XMLoadFloat3(&diagonal))) + planeConstant < 0.0f)
		{
			return true;
		}


	}




	return false;
}
