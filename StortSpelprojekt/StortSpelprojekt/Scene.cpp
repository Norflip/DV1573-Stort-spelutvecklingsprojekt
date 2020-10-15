#include "Scene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"

Scene::Scene(ResourceManager* manager) : input(Input::Instance())
{
	skyboxClass = nullptr;
	renderer = nullptr;
	camera = nullptr;

	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
	resourceManager = manager;
}

Scene::~Scene()
{	
	delete skyboxClass;
	skyboxClass = nullptr;

	delete root;
	root = nullptr;	
}

void Scene::Initialize(Renderer* renderer)
{
  
}

void Scene::InitializeObjects()
{

}

void Scene::Update(const float& deltaTime)
{
	clock.Update();

	//dx::XMFLOAT3 positionA = { 0,0,2 };
	//dx::XMFLOAT3 positionB = { 0, 2,-5};

	//DShape::DrawBox(positionA, { 2,2,2 }, { 0, 1, 1 });
	//DShape::DrawWireBox(positionB, { 4,4,4 }, { 1,0,0 });
	//DShape::DrawSphere({ -4,0,0 }, 1.0f, { 0, 0, 1 });
	//DShape::DrawWireSphere({ -4,0,5 }, 1.0f, { 0,1,0 });
	//DShape::DrawLine(positionA, positionB, { 1,1,0 });
  
	input.UpdateInputs();
	
	root->Update(deltaTime);

	GameClock::Instance().Update();

	guiManager->UpdateAll();
	
	renderer->UpdateTime((float)clock.GetSeconds());
	if (clock.GetSeconds() > 60)
	{
		clock.Restart();
	}
	
	// Press P to recompile shaders
	if (KEY_PRESSED(P))
	{
		std::cout << "Compiling: " << std::endl;
		resourceManager->CompileShaders(renderer->GetDevice());
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
	worldGenerator.DrawShapes();

	renderer->RenderFrame();
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

