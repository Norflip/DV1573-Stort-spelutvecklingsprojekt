#include "IntroScene.h"

IntroScene::IntroScene() : Scene()
{
}

IntroScene::~IntroScene()
{
}

void IntroScene::Initialize(Renderer* renderer)
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

}

void IntroScene::InitializeObjects()
{
	skyboxClass = new Skybox(renderer->GetDevice(), renderer->GetContext(), resourceManager->GetShaderResource("skyboxShader"));
	skyboxClass->GetThisObject()->AddFlag(ObjectFlag::NO_CULL);
}

void IntroScene::Update(const float& deltaTime)
{
}

void IntroScene::FixedUpdate(const float& fixedDeltaTime)
{
}

void IntroScene::Render()
{
	skyboxClass->GetThisObject()->Draw(renderer, camera);

	root->Draw(renderer, camera);
	worldGenerator.DrawShapes();

	renderer->RenderFrame();
}
