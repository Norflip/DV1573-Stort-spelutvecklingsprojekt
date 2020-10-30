#include "stdafx.h"
#include "Scene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "PlayerComp.h"



Scene::Scene() : input(Input::Instance())
{
	skyboxClass = nullptr;
	renderer = nullptr;
	camera = nullptr;
	quit = false;
}

Scene::~Scene()
{
	delete skyboxClass;
	skyboxClass = nullptr;

	delete root;
	root = nullptr;	
}

void Scene::SetDepedencies(ResourceManager* resources, Renderer* renderer, Window* window)
{
	this->resources = resources;
	this->renderer = renderer;
	this->window = window;
	this->pooler = new ObjectPooler(resources);
	this->root = new Object("root");
	this->input.SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void Scene::Update(const float& deltaTime)
{
	clock.Update();
	input.UpdateInputs();
	root->Update(deltaTime);
	GameClock::Instance().Update();

	//renderer->UpdateTime((float)clock.GetSeconds());

	if (clock.GetSeconds() > 60)
	{
		clock.Restart();
	}

	// Press P to recompile shaders
	if (KEY_PRESSED(P))
	{
		std::cout << "Compiling: " << std::endl;
		resources->CompileShaders(renderer->GetDevice());
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
	//worldGenerator.DrawShapes();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
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

	std::cout << (indent + object->GetName()) << std::endl;
	
	if (object->GetTransform().CountChildren() > 0)
	{
		auto children = object->GetTransform().GetChildren();

		for (size_t i = 0; i < children.size(); i++)
		{
			PrintSceneHierarchy(children[i]->GetOwner(), level + 1);
		}
	}
}

bool Scene::Quit()
{
	return quit;
}

