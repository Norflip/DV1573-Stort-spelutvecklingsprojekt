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
	Window* window = renderer->GetOutputWindow();

	windowHeight = window->GetHeight();
	windowWidth = window->GetWidth();

	Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void GameOverScene::InitializeObjects()
{
	Object* cameraObject3 = new Object("camera3", ObjectFlag::ENABLED);
	camera = cameraObject3->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(windowWidth, windowHeight);
	this->player = cameraObject3;
	//cameraObject3->AddComponent<ControllerComponent>();
	AddObject(cameraObject3);

	//Physics& physics = Physics::Instance();

	//dx::XMFLOAT3 pStart = { 10,20,10 };
	//dx::XMVECTOR pStartVec = dx::XMLoadFloat3(&pStart);
	//Object* playerObject = new Object("player", ObjectFlag::ENABLED);
	//Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	////Transform::SetParentChild(playerObject->GetTransform(), cameraObject->GetTransform());
	//this->player = playerObject;
	//camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	//camera->Resize(this->windowWidth, this->windowHeight);
	//cameraObject->GetTransform().SetPosition(pStartVec);
	//playerObject->GetTransform().SetPosition(pStartVec);

	//playerObject->AddComponent<CapsuleColliderComponent>(0.5f, 4.5f, pStart);
	//physics.MutexLock();
	//RigidBodyComponent* rb = playerObject->AddComponent<RigidBodyComponent>(60.f, FilterGroups::PLAYER, FilterGroups::EVERYTHING, true);
	//physics.RegisterRigidBody(rb);
	//physics.MutexUnlock();
	//playerObject->AddComponent<ControllerComponent>(cameraObject);
	//Transform::SetParentChild(playerObject->GetTransform(), cameraObject->GetTransform());
	////playerObject->AddComponent<PlayerComp>(guiManager, 500, 2, 10, 25, 3);

	////AddObject(cameraObject);
	//AddObject(playerObject);
	ShowCursor(true);
	



	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void GameOverScene::InitializeGUI()
{
	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* restart = new GUISprite(*renderer, "Textures/start.png", 100, 200, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quit = new GUISprite(*renderer, "Textures/start.png", 100, 400, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "fps", windowWidth / 2, 50);
	restart->SetActive();
	quit->SetActive();

	guiManager = new GUIManager(renderer, 100);

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
	Scene::FixedUpdate(fixedDeltaTime);
}

void GameOverScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	worldGenerator.DrawShapes();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

