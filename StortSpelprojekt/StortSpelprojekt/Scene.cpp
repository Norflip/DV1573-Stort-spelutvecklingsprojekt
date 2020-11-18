#include "stdafx.h"
#include "Scene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "PlayerComp.h"



Scene::Scene() : input(Input::Instance())
{
	renderer = nullptr;
	camera = nullptr;
}

Scene::~Scene()
{
	delete root;
	root = nullptr;	
}

void Scene::SetDepedencies(ResourceManager* resources, Renderer* renderer, Physics* physics, Window* window)
{
	this->resources = resources;
	this->renderer = renderer;
	this->physics = physics;
	this->window = window;

	this->pooler = new ObjectPooler(resources);
	this->root = new Object("root");
	this->input.SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void Scene::Update(const float& deltaTime)
{




	
	while (!removeQueue.empty())
	{
		Object* obj = removeQueue.front();
		removeQueue.pop();
		Transform::ClearFromHierarchy(obj->GetTransform());
		delete obj;
	}

	clock.Update();
	input.UpdateInputs();
	root->GetTransform().ResetChanged();
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
	//root->FixedUpdate(fixedDeltaTime);
	physics->FixedUpdate(fixedDeltaTime);
}

void Scene::Render()
{
	// skybox draw object
	//renderer->SetRSToCullNone(true);
	//skyboxClass->GetThisObject()->Draw(renderer, camera);

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
	enemyManager->RemoveEnemy(object);
	removeQueue.push(object);
	object->RemoveFlag(ObjectFlag::ENABLED);
	object->AddFlag(ObjectFlag::REMOVED);
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

void Scene::AnimateIcon()
{
	//LPCWSTR icon = L"MAINICON2";
	currentframe++;
	if (currentframe > 35)
		currentframe = 1;

	int frameToDraw = (currentframe / 5)+1;
	if (lastFrame != frameToDraw)
	{
		lastFrame = frameToDraw;
		std::cout << frameToDraw << std::endl;
		std::string narrow_string("test");
		narrow_string.append(std::to_string(frameToDraw));
		std::wstring wide_string = std::wstring(narrow_string.begin(), narrow_string.end());
		const wchar_t* iconText = wide_string.c_str();
		//wchar_t* a = iconText;
		//LPCWSTR lpUserName = *argv;
		SendMessage(window->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(window->GetHINSTANCE(), iconText));
	}
}

