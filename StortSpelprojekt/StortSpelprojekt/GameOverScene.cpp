#include "GameOverScene.h"

GameOverScene::GameOverScene() : Scene()
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

	resourceManager = new ResourceManager;
	resourceManager->InitializeResources(renderer->GetDevice());

	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(window->GetWidth(), window->GetHeight());
	cameraObject->AddComponent<ControllerComponent>();


	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	AddObject(cameraObject);

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void GameOverScene::InitializeObjects()
{
}

void GameOverScene::Update(const float& deltaTime)
{
}

void GameOverScene::FixedUpdate(const float& fixedDeltaTime)
{
}

void GameOverScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	worldGenerator.DrawShapes();

	renderer->RenderFrame();
}
