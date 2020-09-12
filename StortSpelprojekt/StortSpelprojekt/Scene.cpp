#include "Scene.h"
#include "MeshComponent.h"

Scene::Scene() : camera(90.0f, 1.0f)
{

}

Scene::~Scene()
{
}

void Scene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	camera.GetTransform().SetPosition({ 0,0,-10 });

	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.Compile(renderer->GetDevice());

	Object* tmp_obj = new Object();
	tmp_obj->GetTransform().SetPosition({ 0,0,5 });
	
	auto mesh = ShittyOBJLoader::LoadOBJ("Models/cube.obj", renderer->GetDevice());
	auto mat = Material(shader);

	tmp_obj->AddComponent<MeshComponent>(mesh, mat);
	objects.push_back(tmp_obj);
}

void Scene::ProcessInput()
{
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
		if (obj->HasFlag(ObjectFlag::ENABLED | ObjectFlag::VISIBLE))
			obj->Draw(renderer, &camera);
	}

	renderer->EndFrame();
}
