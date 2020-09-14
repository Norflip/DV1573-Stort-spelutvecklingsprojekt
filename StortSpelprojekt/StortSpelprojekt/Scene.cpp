#include "Scene.h"
#include "MeshComponent.h"

Scene::Scene() : camera(60.0f, 1.0f)
{

}

Scene::~Scene()
{
}

void Scene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	//camera.GetTransform().SetPosition({ 0,0, -50 });

	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.Compile(renderer->GetDevice());

	auto mesh = ShittyOBJLoader::LoadOBJ("Models/cube.obj", renderer->GetDevice());
	auto mat = Material(shader);

	const int cubeCount = 40;
	const float toRadians = 0.0174532925f;

	for (size_t i = 0; i < cubeCount; i++)
	{
		float angle = (float)i * (360.0f / cubeCount) * toRadians;
		float x = cosf(angle);
		float z = sinf(angle);

		float length = sqrtf(x * x + z + z);
		x /= length * 10.0f;
		z /= length * 10.0f;

		Object* tmp_obj = new Object("cube");
		tmp_obj->GetTransform().SetPosition({ x, 0, z });

		tmp_obj->AddComponent<MeshComponent>(mesh, mat);
		objects.push_back(tmp_obj);
	}
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
