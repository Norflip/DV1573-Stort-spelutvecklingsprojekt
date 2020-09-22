#include "Scene.h"

Scene::Scene()
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
	
	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.SetVertexShader(L"Shaders/Skeleton_vs.hlsl");
	shader.Compile(renderer->GetDevice());
	
	std::vector<Mesh> zwebMeshes = ZWEBLoader::LoadMeshes(ZWEBLoadType::SkeletonAnimation, "../Models/OrchBody.ZWEB", renderer->GetDevice());
	
	std::vector<Material> zwebMaterials = ZWEBLoader::LoadMaterials("../Models/OrchBody.ZWEB", shader);

	SkeletonAni zwebSkeleton = ZWEBLoader::LoadSkeletonOnly("../Models/OrchAnimation.ZWEB", zwebMeshes[0].GetBoneIDS());

	Object* testMesh = new Object("test"); //Where do you delete??

	testMesh->GetTransform().SetLocalMatrix(zwebMeshes[0].GetWorldMatrix());

	dx::XMFLOAT3 miniScale = dx::XMFLOAT3(0.0625f, 0.0625f, 0.0625f);

	dx::XMFLOAT3 miniTranslation = dx::XMFLOAT3(0, 0, 10);

	testMesh->GetTransform().SetScale(dx::XMLoadFloat3(&miniScale));

	testMesh->GetTransform().SetPosition(dx::XMLoadFloat3(&miniTranslation));

	testMesh->GetTransform().UpdateLocalModelMatrix();

	testMesh->AddComponent<SkeletonMeshComponent>(zwebMeshes[0], zwebMaterials[0]);

	testMesh->GetComponent<SkeletonMeshComponent>()->SetAnimationTrack(zwebSkeleton);
	
	objects.push_back(testMesh);



	/*Mesh mesh = ShittyOBJLoader::Load("Models/cube.obj", renderer->GetDevice());
	Material material = Material(shader);

	Object* tmp_obj = new Object("cube1");
	tmp_obj->GetTransform().SetPosition({ 0, 0, 10 });
	tmp_obj->AddComponent<MeshComponent>(mesh, material);

	objects.push_back(tmp_obj);

	Object* tmp_obj2 = new Object("cube2");
	tmp_obj2->AddComponent<MeshComponent>(mesh, material);
	tmp_obj2->GetTransform().SetPosition({ 0, 0, 4 });

	Transform::SkapaPäron(tmp_obj->GetTransform(), tmp_obj2->GetTransform());
	objects.push_back(tmp_obj2);*/

	//PrintSceneHierarchy();
}

void Scene::Update(const float& deltaTime)
{
	std::vector<Object*> toRemove;

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

	for (auto i = objects.begin(); i < objects.end(); i++)
	{
		Object* obj = (*i);
		//if (obj->HasFlag(ObjectFlag::ENABLED | ObjectFlag::VISIBLE))
		obj->Draw(renderer, camera);
	}

	


	renderer->EndFrame();
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
