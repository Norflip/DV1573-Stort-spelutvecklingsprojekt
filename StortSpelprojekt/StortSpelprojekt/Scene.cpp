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
	
	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.Compile(renderer->GetDevice());

	Mesh mesh = ShittyOBJLoader::Load("Models/cube.obj", renderer->GetDevice());
	Material material = Material(shader);

	Object* tmp_obj = new Object("cube1");
	tmp_obj->GetTransform().SetPosition({ 0, 0, 10 });
	tmp_obj->AddComponent<MeshComponent>(mesh, material);

	objects.push_back(tmp_obj);

	Object* tmp_obj2 = new Object("cube2");
	tmp_obj2->AddComponent<MeshComponent>(mesh, material);
	tmp_obj2->GetTransform().SetPosition({ 0, 0, 4 });

	Transform::SkapaPäron(tmp_obj->GetTransform(), tmp_obj2->GetTransform());
	objects.push_back(tmp_obj2);

	//PrintSceneHierarchy();
}

void Scene::Update(const float& deltaTime)
{
	std::vector<Object*> toRemove;
	input.UpdateInputs();
	if (input.GetKeyDown(DirectX::Keyboard::Keys::Q))
	{
		std::cout<<"FAKK";
	}
	
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
