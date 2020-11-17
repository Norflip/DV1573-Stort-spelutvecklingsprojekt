#include "stdafx.h"
#include "Engine.h"
#include "FogRenderPass.h"
#include "FXAARenderPass.h"
#include "SkyboxRenderPass.h"

Engine* Engine::Instance = nullptr;

Engine::Engine(HINSTANCE hInstance) : window(hInstance), activeScene(nullptr), sceneSwitch(-1)
{
	this->Instance = this;

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));

	window.Open(1600, 900);


	renderer = new Renderer();
	renderer->Initialize(&window);

	resourceManager = new ResourceManager();
	resourceManager->InitializeResources(renderer->GetDevice());

	physics = new Physics();
	physics->Initialize();

	renderer->AddRenderPass(new SkyboxRenderPass(-10, resourceManager));
	renderer->AddRenderPass(new FogRenderPass(0, resourceManager)); //VIKTOR
	renderer->AddRenderPass(new FXAARenderPass(1, resourceManager));

	RegisterScene(SceneIndex::INTRO,	new IntroScene());
	RegisterScene(SceneIndex::GAME_OVER,new GameOverScene());
	RegisterScene(SceneIndex::GAME,		new GameScene());
	RegisterScene(SceneIndex::WIN,		new WinScene());

	SwitchScene(SceneIndex::INTRO);
}

Engine::~Engine()
{
	for (size_t i = 0; i < scenes.size(); i++)
	{
		if (scenes[i])
		{
			delete scenes[i];
		}
	}

	delete renderer;
	delete resourceManager;
	delete physics;
}

void Engine::Run()
{
	this->running = true;
#if MULTITHREAD_PHYSICS
	std::thread fixedLoopThread (Engine::FixedUpdateLoop, this);
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
			if (sceneSwitch != -1)
			{
				auto sceneIt = this->scenes.find(sceneSwitch);
				assert(sceneIt != scenes.end());

				Scene* previous = activeScene;
				if (previous != nullptr)
				{
					previous->OnDeactivate();
				}

				activeScene = (*sceneIt).second;
				activeScene->OnActivate();
				sceneSwitch = -1;
				std::cout << "switching scene" << std::endl;
			}

			auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTimePoint);
			float currentTime = static_cast<float>(elapsed.count() / 1000.0f);
			float deltaTime = currentTime - timeLastFrame;

			if (activeScene != nullptr)
			{
				float deltaTime = currentTime - timeLastFrame;
				activeScene->Update(deltaTime);

#if !MULTITHREAD_PHYSICS
				fixedTimeAccumulation += deltaTime;

				while (fixedTimeAccumulation >= TARGET_FIXED_DELTA)
				{
					// FIXED UPDATE
					fixedTimeAccumulation -= TARGET_FIXED_DELTA;
					activeScene->FixedUpdate(TARGET_FIXED_DELTA);
				}
#endif

				activeScene->Render();
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

void Engine::RegisterScene(size_t id, Scene* scene)
{
	auto sceneIt = this->scenes.find(id);
	assert(sceneIt == scenes.end());

	scene->SetDepedencies(resourceManager, renderer, physics, renderer->GetOutputWindow());
	scene->Initialize();
	
	this->scenes.insert({ id, scene });
}

void Engine::UnregisterScene(size_t id)
{
	auto sceneIt = this->scenes.find(id);
	if (sceneIt != scenes.end())
		scenes.erase(sceneIt);
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
