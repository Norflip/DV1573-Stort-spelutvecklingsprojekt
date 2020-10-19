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

	spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	guiManager = new GUIManager(renderer);
	renderer->SetGUIManager(guiManager);

	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	windowHeight = window->GetHeight();
	windowWidth = window->GetWidth();


	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void GameOverScene::InitializeObjects()
{
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(windowWidth, windowHeight);
	cameraObject->AddComponent<ControllerComponent>();
	AddObject(cameraObject);

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void GameOverScene::InitializeGUI()
{
	spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* restart = new GUISprite(*renderer, "Textures/start.png", 100, 200, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quit = new GUISprite(*renderer, "Textures/start.png", 100, 400, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	restart->SetActive();
	quit->SetActive();

	guiManager = new GUIManager(renderer);
	renderer->SetGUIManager(guiManager);
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(restart, "normalSprite");
	guiManager->AddGUIObject(quit, "buttonSprite");
}

void GameOverScene::OnActivate()
{
	nextScene = WIN;
	InitializeGUI();
	InitializeObjects();
}

void GameOverScene::OnDeactivate()
{
}

void GameOverScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);

	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	guiManager->UpdateAll();
}

void GameOverScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
}

void GameOverScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	worldGenerator.DrawShapes();

	renderer->RenderFrame();
}
