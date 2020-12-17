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
	delete guiManager;
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
	float windowHeight = FCAST(window->GetHeight());
	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* lose = new GUISprite(*renderer, "Textures/Died.png", windowWidth / 2 - 120, 100, 0, DrawDirection::Default, ClickFunction::NotClickable);
	GUISprite* restart = new GUISprite(*renderer, "Textures/Restart.png", 100, 200, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quit = new GUISprite(*renderer, "Textures/Exit.png", 100, 400, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* returnToMenu = new GUISprite(*renderer, "Textures/BackButton.png", 100, 600, 0, DrawDirection::Default, ClickFunction::Clickable);
	
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	GUIFont* deathReason = new GUIFont(*renderer, "death", windowWidth / 2-240, 250);
	deathReason->SetFontSize({ 1,1 });
	GUIFont* scoreDisplay = new GUIFont(*renderer, "info", windowWidth / 2 - 460, windowHeight/2-200);
	scoreDisplay->SetFontSize({ 0.6f,0.6f });

	restart->SetActivated();
	quit->SetActivated();
	returnToMenu->SetActivated();

	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(lose, "lose");
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(restart, "restart");
	guiManager->AddGUIObject(quit, "quit");
	guiManager->AddGUIObject(returnToMenu, "returnToMenu");

	guiManager->AddGUIObject(deathReason, "death");
	guiManager->AddGUIObject(scoreDisplay, "scoreDisplay");
}

void GameOverScene::OnActivate()
{
	renderer->AddRenderPass(guiManager);
	Input::Instance().FreeMouse();
	Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	//ShowCursor(true);

	SaveHandler::RemoveSave();
}

void GameOverScene::OnDeactivate()
{
	renderer->RemoveRenderPass(guiManager);
}

void GameOverScene::Update(const float& deltaTime)
{
	if (!Input::Instance().GetIsVisible())
		ShowCursor(true);

	Scene::Update(deltaTime);
	
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));
	std::string deathReason = "Killed by ";
	deathReason += MetaProgress::Instance().GetKilledBy();
	static_cast<GUIFont*>(guiManager->GetGUIObject("death"))->SetString(deathReason);

	//GUIFont* test = (GUIFont*)guiManager->GetGUIObject("scoreDisplay");
	static_cast<GUIFont*>(guiManager->GetGUIObject("scoreDisplay"))->SetString(MetaProgress::Instance().GetInfo());


	if(static_cast<GUISprite*>(guiManager->GetGUIObject("quit"))->IsClicked())
	{
		OnDeactivate();
		Engine::Instance->Exit();
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("restart"))->IsClicked())
	{
		SaveHandler::RemoveSave();
		Engine::Instance->start = true;
		Engine::Instance->SwitchScene(SceneIndex::GAME);
		return;
	}
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("returnToMenu"))->IsClicked())
	{
		AudioMaster::Instance().PlaySoundEvent("menusound");
		Engine::Instance->start = true;
		Engine::Instance->SwitchScene(SceneIndex::INTRO);
		return;
	}

	guiManager->UpdateAll();
}

void GameOverScene::FixedUpdate(const float& fixedDeltaTime)
{
	//Scene::FixedUpdate(fixedDeltaTime);
}
