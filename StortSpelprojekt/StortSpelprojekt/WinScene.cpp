#include "stdafx.h"
#include "WinScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"
#include "Engine.h"

WinScene::WinScene() : Scene("WinScene")
{
	
}

WinScene::~WinScene()
{
	delete guiManager;
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
	AddObjectToRoot(cameraObject);
}

void WinScene::InitializeGUI()
{
	float windowWidth = static_cast<float>(window->GetWidth());
	float windowHeight = FCAST(window->GetHeight());
	GUISprite* win = new GUISprite(*renderer, "Textures/Win.png", windowWidth / 2 - 120, 100, 0, DrawDirection::Default, ClickFunction::NotClickable);
	GUISprite* restart = new GUISprite(*renderer, "Textures/Restart.png", 100, 200, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quit = new GUISprite(*renderer, "Textures/Exit.png", 100, 400, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	restart->SetActivated();
	quit->SetActivated();
	GUIFont* scoreDisplay = new GUIFont(*renderer, "info", windowWidth / 2 - 460, windowHeight / 2 - 200);
	scoreDisplay->SetFontSize({ 0.6f,0.6f });

	guiManager = new GUIManager(renderer, 100);

	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(win, "win");
	guiManager->AddGUIObject(restart, "restart");
	guiManager->AddGUIObject(quit, "quit");

	guiManager->AddGUIObject(scoreDisplay, "scoreDisplay");
}

void WinScene::OnActivate()
{
	input.SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	//ShowCursor(true);
	renderer->AddRenderPass(guiManager);
}

void WinScene::OnDeactivate()
{
	AudioMaster::Instance().StopSoundEvent("menusound");
	renderer->RemoveRenderPass(guiManager);
}

void WinScene::Update(const float& deltaTime)
{
	if (!Input::Instance().GetIsVisible())
		ShowCursor(true);

	Scene::Update(deltaTime);
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));
	static_cast<GUIFont*>(guiManager->GetGUIObject("scoreDisplay"))->SetString(MetaProgress::Instance().GetInfo());
	if (static_cast<GUISprite*>(guiManager->GetGUIObject("quit"))->IsClicked())
	{
		AudioMaster::Instance().StopSoundEvent("menusound");
		OnDeactivate();
		Engine::Instance->Exit();
		return;
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("restart"))->IsClicked())
	{
		guiManager->ChangeGuiGroup(GuiGroup::Load);
		AudioMaster::Instance().StopSoundEvent("menusound");
		Engine::Instance->SwitchScene(SceneIndex::GAME);
		Engine::Instance->start = true;
		return;
		//AudioMaster::Instance().StopSoundEvent("menusound");
		//Engine::Instance->SwitchScene(SceneIndex::INTRO);
		//return;
	}

	guiManager->UpdateAll();
}

void WinScene::Render()
{
	root->Draw(renderer, camera);
	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

