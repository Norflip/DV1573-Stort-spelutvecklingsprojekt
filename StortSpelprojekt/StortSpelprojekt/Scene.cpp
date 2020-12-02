#include "stdafx.h"
#include "Scene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "PlayerComp.h"
#include "Engine.h"

Scene::Scene(const std::string& debugName) : input(Input::Instance()), debugName(debugName)
{
	renderer = nullptr;
	camera = nullptr;
	this->root = new Object("root");
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

	this->input.SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void Scene::Update(const float& deltaTime)
{

		while (!removeQueue.empty())
		{
			Object* obj = removeQueue.front();
			removeQueue.pop();
			Object::RemoveFromHierarchy(obj);
			delete obj;
		}

		clock.Update();

		input.UpdateInputs();
		root->GetTransform().SetChanged(true);
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
	// We need a check when everything is loaded
	if (firstFrame)
	{
		root->FixedUpdate(fixedDeltaTime);
		//std::cout << GameClock::Instance().GetSeconds() << std::endl;
		physics->FixedUpdate(fixedDeltaTime);
	}
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

void Scene::AddObjectToRoot(Object* object)
{
	Object::AddToHierarchy(root, object);
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
	
	if (object->CountChildren() > 0)
	{
		auto children = object->GetChildren();

		for (size_t i = 0; i < children.size(); i++)
		{
			PrintSceneHierarchy(children[i], level + 1);
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
		std::string narrow_string("test");
		narrow_string.append(std::to_string(frameToDraw));
		std::wstring wide_string = std::wstring(narrow_string.begin(), narrow_string.end());
		const wchar_t* iconText = wide_string.c_str();

		HMODULE& hmod = Engine::Instance->GetResources()->GetHMODULE();
		SendMessage(window->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)LoadIconW(hmod, iconText));
	}
}

void Scene::DebugCheckForObjectIDConflict(Object* current, std::unordered_set<size_t>& registry, size_t& conflicCounter) const
{
	auto find = registry.find(current->GetID());

	if (find == registry.end())
		registry.insert(current->GetID());
	else
	{
		conflicCounter++;
	}

	auto children = current->GetChildren();
	for (size_t i = 0; i < children.size(); i++)
		DebugCheckForObjectIDConflict(children[i], registry, conflicCounter);
}

