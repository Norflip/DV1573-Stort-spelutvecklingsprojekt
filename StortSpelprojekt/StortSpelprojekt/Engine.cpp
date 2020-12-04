#include "stdafx.h"
#include "Engine.h"
#include "FogRenderPass.h"
#include "FXAARenderPass.h"
#include "SkyboxRenderPass.h"
#include "GlowRenderPass.h"

#include "Imgui\imgui.h"
#include "Imgui\imgui_impl_win32.h"
#include "Imgui\imgui_impl_dx11.h"
#include "SaveHandler.h"

Engine* Engine::Instance = nullptr;

Engine::Engine(HINSTANCE hInstance) : window(hInstance), activeSceneIndex(-1), sceneSwitch(-1)
{
	this->Instance = this;

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	window.Open(1920, 1080);

	renderer = new Renderer();
	renderer->Initialize(&window);

	resourceManager = new ResourceManager();
	resourceManager->InitializeResources(renderer->GetDevice());
	
	physics = new Physics();
	physics->Initialize();

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(window.GetHWND());
	ImGui_ImplDX11_Init(renderer->GetDevice(), renderer->GetContext());
	ImGui::StyleColorsDark();

	//renderer->AddRenderPass(new SkyboxRenderPass(-10, resourceManager));
	renderer->AddRenderPass(new FogRenderPass(0, resourceManager));
	renderer->AddRenderPass(new FXAARenderPass(1, resourceManager));
	renderer->AddRenderPass(new GlowRenderPass(2, resourceManager));

	RegisterScene(SceneIndex::INTRO,	new IntroScene());
	RegisterScene(SceneIndex::GAME_OVER,new GameOverScene());
	RegisterScene(SceneIndex::GAME,		new GameScene());
	RegisterScene(SceneIndex::WIN,		new WinScene());
	RegisterScene(SceneIndex::CREDITS,	new CreditsScene());

	SaveHandler::RemoveSave();

	SwitchScene(SceneIndex::INTRO);
}

Engine::~Engine()
{
	for (size_t i = 0; i < SCENE_COUNT; i++)
	{
		delete scenes[i];
		scenes[i] = nullptr;
	}

	delete renderer;
	delete resourceManager;
	delete physics;
}

void Engine::Run()
{
	this->running = true;
#if MULTITHREAD_PHYSICS
	std::thread fixedLoopThread(Engine::FixedUpdateLoop, this);
#endif

	auto startTimePoint = std::chrono::high_resolution_clock::now();
	float timeLastFrame = 0.0f;
	float fixedTimeAccumulation = 0.0f;

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (running)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				Exit();
		}
		else
		{
			if (sceneSwitch >= 0 && sceneSwitch < SCENE_COUNT)
			{
				if (activeSceneIndex != -1)
				{
					scenes[activeSceneIndex]->OnDeactivate();
				}

				activeSceneIndex = sceneSwitch;
				scenes[activeSceneIndex]->OnActivate();
				sceneSwitch = -1;
				
				//std::cout << "switching scene" << std::endl;
			}

			auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTimePoint);
			float currentTime = static_cast<float>(elapsed.count() / 1000.0f);
			float deltaTime = currentTime - timeLastFrame;

			if (activeSceneIndex >= 0 && activeSceneIndex < SCENE_COUNT)
			{
				Scene* scene = scenes[activeSceneIndex];
				float deltaTime = currentTime - timeLastFrame;
				scene->Update(deltaTime);

#if !MULTITHREAD_PHYSICS
				fixedTimeAccumulation += deltaTime;

				while (fixedTimeAccumulation >= TARGET_FIXED_DELTA)
				{
					// FIXED UPDATE
					fixedTimeAccumulation -= TARGET_FIXED_DELTA;
					scene->FixedUpdate(TARGET_FIXED_DELTA);
				}
#endif
				scene->Render();
			}

			timeLastFrame = currentTime;
		}
	}

#if MULTITHREAD_PHYSICS
	fixedLoopThread.join();
#endif

	CoUninitialize();
}

void Engine::Exit()
{
	running = false;
}

Scene* Engine::GetActiveScene() const
{
	Scene* active = nullptr;
	if (activeSceneIndex >= 0 && activeSceneIndex < SCENE_COUNT)
		active = scenes[activeSceneIndex];
	return active;
}

void Engine::RegisterScene(size_t id, Scene* scene)
{
	assert(id >= 0 && id < SCENE_COUNT);
	scene->SetDepedencies(resourceManager, renderer, physics, renderer->GetOutputWindow());
	scene->Initialize();
	scenes[id] = scene;
}

void Engine::SwitchScene(size_t id)
{
	this->sceneSwitch = id;
}

void Engine::OnIMGUIFrame()
{

	static bool show_another_window = false;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("TMP Imgui window");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::Checkbox("Another Window", &show_another_window);
		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;

		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	if (activeSceneIndex >= 0 && activeSceneIndex < SCENE_COUNT)
	{
		Scene* scene = scenes[activeSceneIndex];
		scene->OnIMGUIFrame();
	}
}

void Engine::FixedUpdateLoop(Engine* engine)
{
	auto startTimePoint = std::chrono::high_resolution_clock::now();
	float timeLastFrame = 0.0f;
	float fixedTimeAccumulation = 0.0f;

	while (engine->IsRunning())
	{
		auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTimePoint);
		float currentTime = static_cast<float>(elapsed.count() / 1000.0f);
		float deltaTime = currentTime - timeLastFrame;

		fixedTimeAccumulation += deltaTime;
		while (fixedTimeAccumulation >= TARGET_FIXED_DELTA)
		{
			// FIXED UPDATE
			fixedTimeAccumulation -= TARGET_FIXED_DELTA;

			Scene* activeScene = engine->GetActiveScene();

			if (activeScene != nullptr)
				activeScene->FixedUpdate(TARGET_FIXED_DELTA);
		}

		timeLastFrame = currentTime;
	}
}
