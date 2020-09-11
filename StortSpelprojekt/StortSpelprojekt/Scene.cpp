#include "Scene.h"

Scene::Scene() : camera(90.0f, 1.0f)
{

}

Scene::~Scene()
{
}

void Scene::Initialize(DXHandler* dxHandler, Renderer* renderer)
{
	this->dxHandler = dxHandler;
	this->renderer = renderer;

	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.Compile(dxHandler->GetDevice());

	Object tmp_obj;
	tmp_obj.SetMesh(ShittyOBJLoader::LoadOBJ("Models/cube.obj", dxHandler->GetDevice()));
	tmp_obj.SetMaterial(Material(shader));
	tmp_obj.GetTransform().SetPosition({ 0,0,5 });

	objects.push_back(tmp_obj);
}

void Scene::ProcessInput()
{
}

void Scene::Update(const float& deltaTime)
{
	(*objects.begin()).GetTransform().Rotate(2.0f * deltaTime, 2.0f * deltaTime, 0.0f);
}

void Scene::FixedUpdate(const float& fixedDeltaTime)
{
	//Log::Add(std::to_string(fixedDeltaTime));
}

void Scene::Render()
{
	renderer->BeginFrame();
	ObjectFlag reqflag = (ObjectFlag::ENABLED | ObjectFlag::VISIBLE);

	for (auto i = objects.begin(); i < objects.end(); i++)
	{
		auto obj = (*i);

		if (obj.HasFlag(reqflag))
		{
			obj.GetMaterial().BindToContext(dxHandler->GetContext());
			obj.Draw(renderer, &camera);
		}
	}

	renderer->EndFrame();
}
