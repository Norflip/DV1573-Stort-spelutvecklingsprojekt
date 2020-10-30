#include "GameOverScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"

GameOverScene::GameOverScene(ResourceManager* manager) : Scene(manager)
{
}

GameOverScene::~GameOverScene()
{
}

void GameOverScene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	// Should change values on resize event
	this->window = renderer->GetOutputWindow();

	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void GameOverScene::InitializeObjects()
{
	
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(window->GetWidth(), window->GetHeight());
	this->player = cameraObject;
	//cameraObject3->AddComponent<ControllerComponent>();
	AddObject(cameraObject);

	ShowCursor(true);
	Input::Instance().SetMouseMode(dx::Mouse::MODE_ABSOLUTE);

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
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
	renderer->AddRenderPass(guiManager);
}

void GameOverScene::OnActivate()
{
	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
	nextScene = LOSE;
	InitializeGUI();
	InitializeObjects();
	ShowCursor(true);
}

void GameOverScene::OnDeactivate()
{
	renderer->RemoveRenderPass(guiManager);
	delete root;
	root = nullptr;
}

void GameOverScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	
	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	if(static_cast<GUISprite*>(guiManager->GetGUIObject("quit"))->IsClicked())
	{
		quit = true;
	}

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("restart"))->IsClicked())
	{
		nextScene = GAME;
	}

	guiManager->UpdateAll();
}

void GameOverScene::FixedUpdate(const float& fixedDeltaTime)
{
	//Scene::FixedUpdate(fixedDeltaTime);
}

void GameOverScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

