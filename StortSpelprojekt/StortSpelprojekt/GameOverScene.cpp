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

	//resourceManager = new ResourceManager;
	//resourceManager->InitializeResources(renderer->GetDevice());

	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(window->GetWidth(), window->GetHeight());
	cameraObject->AddComponent<ControllerComponent>();


	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	AddObject(cameraObject);

}

void GameOverScene::InitializeObjects()
{
	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void GameOverScene::InitializeGUI()
{
	spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* normalSprite = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::BottomLeft, ClickFunction::Clickable);
	GUISprite* buttonSprite = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::BottomRight, ClickFunction::Clickable);
	GUISprite* normalSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::TopLeft, ClickFunction::Clickable);
	//GUISprite* buttonSprite2 = new GUISprite(*renderer, "Textures/EquipmentBox.png", 0, 0, DrawDirection::TopRight, ClickFunction::Clickable);
	//GUIFont* fpsDisplay = new GUIFont(*renderer, "test", 300, 300);
	normalSprite->SetActive();

	guiManager = new GUIManager(renderer);
	renderer->SetGUIManager(guiManager);
	//guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(normalSprite, "normalSprite");
	guiManager->AddGUIObject(buttonSprite, "buttonSprite");
	guiManager->AddGUIObject(normalSprite2, "normalSprite2");
	//guiManager->AddGUIObject(buttonSprite2, "buttonSprite2");
	guiManager->GetGUIObject("normalSprite")->SetPosition(100, 100);
}

void GameOverScene::OnActivate()
{
	InitializeGUI();
	InitializeObjects();
}

void GameOverScene::OnDeactivate()
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
