#include "stdafx.h"
#include "Scene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "PlayerComp.h"

Scene::Scene(const std::string& debugName) : input(Input::Instance()), debugName(debugName)
{
	renderer = nullptr;
	camera = nullptr;
	this->root = new Object("root");
	resize = false;
}

Scene::~Scene()
{
	delete root;
	root = nullptr;
}

void Scene::OnResize(size_t width, size_t height)
{
	this->camera->UpdateProjectionMatrix(width, height, this->camera->GetFOV(), this->camera->GetNearZ(), this->camera->GetFarZ());
	resize = true;
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
}

void Scene::FixedUpdate(const float& fixedDeltaTime)
{
	// Delays gamestart so the physics can load properly
	if (firstFrame)
	{
		guiManager->GetGUIObject("loading")->SetVisible(true);
		if (delayTimer > physicsDelay && !sceneSwitch)
		{
			root->FixedUpdate(fixedDeltaTime);
			//std::cout << GameClock::Instance().GetSeconds() << std::endl;
			physics->FixedUpdate(fixedDeltaTime);

			if (delayTimer > (physicsDelay + loadScreenDelay))
			{
				player->AddFlag(ObjectFlag::ENABLED);
				guiManager->GetGUIObject("loading")->SetVisible(false);

			}
		}
		delayTimer += GameClock::Instance().GetFixedFrameTime();
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

	//std::cout << (indent + object->GetName()) << std::endl;

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

	int frameToDraw = (currentframe / 5) + 1;
	if (lastFrame != frameToDraw)
	{
		lastFrame = frameToDraw;
		std::string narrow_string("test");
		narrow_string.append(std::to_string(frameToDraw));
		std::wstring wide_string = std::wstring(narrow_string.begin(), narrow_string.end());
		const wchar_t* iconText = wide_string.c_str();
		SendMessage(window->GetHWND(), WM_SETICON, ICON_SMALL, (LPARAM)LoadIcon(window->GetHINSTANCE(), iconText));
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


