#include "stdafx.h"
#include "GameOverScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"
#include "SaveHandler.h"

GameOverScene::GameOverScene() : Scene("GameOverScene")
{
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Initialize()
{
	InitializeGUI();
	InitializeObjects();
}

void GameOverScene::InitializeObjects()
{
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);

	camera = cameraObject->AddComponent<CameraComponent>(window->GetWidth(), window->GetHeight(),60.0f);
	this->player = cameraObject;
	//cameraObject3->AddComponent<ControllerComponent>();
	AddObjectToRoot(cameraObject);
}

void GameOverScene::InitializeGUI()
{
	float windowWidth = FCAST(window->GetWidth());
	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* lose = new GUISprite(*renderer, "Textures/Died.png", windowWidth / 2 - 120, 100, 0, DrawDirection::Default, ClickFunction::NotClickable);
	GUISprite* restart = new GUISprite(*renderer, "Textures/Restart.png", 100, 200, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quit = new GUISprite(*renderer, "Textures/Exit.png", 100, 400, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	restart->SetActivated();
	quit->SetActivated();

	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(lose, "lose");
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(restart, "restart");
	guiManager->AddGUIObject(quit, "quit");
}

void GameOverScene::OnActivate()
{
	renderer->AddRenderPass(guiManager);
	Input::Instance().FreeMouse();
	ShowCursor(true);
	Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);

	SaveHandler::RemoveSave();
}

void GameOverScene::OnDeactivate()
{
	renderer->RemoveRenderPass(guiManager);
}

void GameOverScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	if(static_cast<GUISprite*>(guiManager->GetGUIObject("quit"))->IsClicked())
	{
		Engine::Instance->Exit();
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("restart"))->IsClicked())
	{
		Engine::Instance->SwitchScene(SceneIndex::GAME);
		Engine::Instance->start = true;
	}

	guiManager->UpdateAll();
}

void GameOverScene::FixedUpdate(const float& fixedDeltaTime)
{
	//Scene::FixedUpdate(fixedDeltaTime);
}
