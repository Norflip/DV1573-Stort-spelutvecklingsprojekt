#include "stdafx.h"
#include "WinScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"

WinScene::WinScene() : Scene()
{
	
}

WinScene::~WinScene()
{
}

void WinScene::Initialize()
{
	InitializeGUI();
	InitializeObjects();
}

void WinScene::InitializeObjects()
{
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(window->GetWidth(), window->GetHeight(),60.0f);
	this->player = cameraObject;
	//cameraObject3->AddComponent<ControllerComponent>();
	AddObject(cameraObject);

	ShowCursor(true);
	Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);



	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resources->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void WinScene::InitializeGUI()
{
	float windowWidth = static_cast<float>(window->GetWidth());

	GUISprite* win = new GUISprite(*renderer, "Textures/Win.png", windowWidth / 2 - 120, 100, 0, DrawDirection::Default, ClickFunction::NotClickable);
	GUISprite* restart = new GUISprite(*renderer, "Textures/Restart.png", 100, 200, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quit = new GUISprite(*renderer, "Textures/Exit.png", 100, 400, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	restart->SetActivated();
	quit->SetActivated();

	guiManager = new GUIManager(renderer, 100);

	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(win, "win");
	guiManager->AddGUIObject(restart, "restart");
	guiManager->AddGUIObject(quit, "quit");
}

void WinScene::OnActivate()
{
	renderer->AddRenderPass(guiManager);
}

void WinScene::OnDeactivate()
{
	renderer->RemoveRenderPass(guiManager);
}

void WinScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("quit"))->IsClicked())
	{
		Engine::Instance->Exit();
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("restart"))->IsClicked())
	{
		Engine::Instance->SwitchScene(SceneIndex::GAME);
	}

	guiManager->UpdateAll();
}

void WinScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
}

void WinScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

