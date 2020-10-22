#include "IntroScene.h"
#include "RenderPass.h"
#include "GUISprite.h"
#include "GUIFont.h"

IntroScene::IntroScene(ResourceManager* manager) : Scene(manager)
{
}

IntroScene::~IntroScene()
{
}

void IntroScene::Initialize(Renderer* renderer)
{
	this->renderer = renderer;

	/*Physics& physics = Physics::Instance();
	physics.Initialize({ 0, -10.0f, 0 });*/

	// Should change values on resize event
	Window* window = renderer->GetOutputWindow();

	windowHeight = window->GetHeight();
	windowWidth = window->GetWidth();


	//Input::Instance().SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
	input.SetWindow(window->GetHWND(), window->GetHeight(), window->GetWidth());
}

void IntroScene::InitializeObjects()
{
	Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
	camera = cameraObject->AddComponent<CameraComponent>(60.0f, true);
	camera->Resize(windowWidth, windowHeight);
	//cameraObject2->AddComponent<ControllerComponent>();
	this->player = cameraObject;
	AddObject(cameraObject);

	//Physics& physics = Physics::Instance();
	//dx::XMFLOAT3 pStart = { 10,20,10 };
	//dx::XMVECTOR pStartVec = dx::XMLoadFloat3(&pStart);
	//Object* playerObject = new Object("player", ObjectFlag::ENABLED);
	//Object* cameraObject = new Object("camera", ObjectFlag::ENABLED);
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
	//Transform::SetParentChild(playerObject->GetTransform(),cameraObject->GetTransform());
	////playerObject->AddComponent<PlayerComp>(guiManager, 500, 2, 10, 25, 3);

	////AddObject(cameraObject);
	//AddObject(playerObject);
	ShowCursor(true); 

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void IntroScene::InitializeGUI()
{
	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* startSprite = new GUISprite(*renderer, "Textures/start.png", 100, 100, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* optionSprite = new GUISprite(*renderer, "Textures/options.png", 100, 300, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* loreSprite = new GUISprite(*renderer, "Textures/lord.png", 100, 500, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quitSprite = new GUISprite(*renderer, "Textures/Restart.png", 100, 700, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "test", windowWidth / 2, 50);
	startSprite->SetActive();
	optionSprite->SetActive();
	quitSprite->SetActive();
	loreSprite->SetActive();

	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(fpsDisplay, "fps");
	guiManager->AddGUIObject(startSprite, "startSprite");
	guiManager->AddGUIObject(optionSprite, "optionSprite");
	guiManager->AddGUIObject(quitSprite, "quitSprite");
	guiManager->AddGUIObject(loreSprite, "loreSprite");
	renderer->AddRenderPass(guiManager);
}

void IntroScene::OnActivate()
{
	root = new Object("sceneRoot", ObjectFlag::DEFAULT);
	nextScene = INTRO;
	input.SetMouseMode(dx::Mouse::MODE_ABSOLUTE);
	InitializeGUI();
	InitializeObjects();
}

void IntroScene::OnDeactivate()
{
	renderer->RemoveRenderPass(guiManager);
	delete root;
	root = nullptr;
}

void IntroScene::Update(const float& deltaTime)
{
	Scene::Update(deltaTime);
	//GameClock::Instance().Update();

	static_cast<GUIFont*>(guiManager->GetGUIObject("fps"))->SetString(std::to_string((int)GameClock::Instance().GetFramesPerSecond()));

	if (static_cast<GUISprite*>(guiManager->GetGUIObject("startSprite"))->IsClicked())
	{
		nextScene = GAME;
	}
	else if (static_cast<GUISprite*>(guiManager->GetGUIObject("quitSprite"))->IsClicked())
	{
		quit = true;
	}
	guiManager->UpdateAll();

}

void IntroScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
}

void IntroScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	worldGenerator.DrawShapes();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

