#include "stdafx.h"
#include "Engine.h"
#include "FogRenderPass.h"
#include "FXAARenderPass.h"
#include "SkyboxRenderPass.h"


#include "SaveHandler.h"

Engine* Engine::Instance = nullptr;

Engine::Engine(HINSTANCE hInstance) : window(hInstance), activeSceneIndex(-1), sceneSwitch(-1)
{
	this->Instance = this;

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	window.Open(1920, 1080);

	SaveState state;
	if (SaveHandler::TryLoad(state))
	{
		std::cout << "seed: " << state.seed << std::endl;

		std::cout << "FOOD: " << state.playerFood << std::endl;
		std::cout << "FUEL: " << state.playerFuel << std::endl;
		std::cout << "HP: " << state.playerHealth << std::endl;
	}
	else
	{
		SaveHandler::CreateNew();
	}
	

	renderer = new Renderer();
	renderer->Initialize(&window);

	resourceManager = new ResourceManager();
	resourceManager->InitializeResources(renderer->GetDevice());
	
	physics = new Physics();
	physics->Initialize();

	renderer->AddRenderPass(new SkyboxRenderPass(-10, resourceManager));
	renderer->AddRenderPass(new FogRenderPass(0, resourceManager));
	renderer->AddRenderPass(new FXAARenderPass(1, resourceManager));

	SetScene(SceneIndex::INTRO,		new IntroScene());
	SetScene(SceneIndex::GAME_OVER, new GameOverScene());
	SetScene(SceneIndex::GAME,		new GameScene());
	SetScene(SceneIndex::WIN,		new WinScene());

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
				std::cout << "switching scene" << std::endl;
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

void Engine::SetScene(size_t id, Scene* scene)
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
