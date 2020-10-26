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

	ShowCursor(true); 

	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void IntroScene::InitializeGUI()
{
	//spriteBatch = new DirectX::SpriteBatch(renderer->GetContext());
	GUISprite* titleSprite = new GUISprite(*renderer, "Textures/OnceUponATime.png", windowWidth / 2 - 100, 100, 0, DrawDirection::Default, ClickFunction::NotClickable);
	GUISprite* startSprite = new GUISprite(*renderer, "Textures/Start.png", 100, 100, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* optionSprite = new GUISprite(*renderer, "Textures/Options.png", 100, 300, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* loreSprite = new GUISprite(*renderer, "Textures/Lore.png", 100, 500, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUISprite* quitSprite = new GUISprite(*renderer, "Textures/Exit.png", 100, 700, 0, DrawDirection::Default, ClickFunction::Clickable);
	GUIFont* fpsDisplay = new GUIFont(*renderer, "test", windowWidth / 2, 50);
	startSprite->SetActive();
	optionSprite->SetActive();
	quitSprite->SetActive();
	loreSprite->SetActive();

	guiManager = new GUIManager(renderer, 100);
	guiManager->AddGUIObject(titleSprite, "title");
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
	guiManager->ClearGui();

}

void IntroScene::FixedUpdate(const float& fixedDeltaTime)
{
	Scene::FixedUpdate(fixedDeltaTime);
}

void IntroScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	//worldGenerator.DrawShapes();

	renderer->RenderFrame(camera, (float)clock.GetSeconds());
}

