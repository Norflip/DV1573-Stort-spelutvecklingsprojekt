#include "stdafx.h"
#include "Engine.h"


Engine const * Engine::Instance = nullptr;

Engine::Engine(HINSTANCE hInstance) : window(hInstance), activeScene(nullptr)
{
	this->Instance = this;
	window.Open(1600, 900);
	renderer = new Renderer();
	renderer->Initialize(&window);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));		

	resourceManager = new ResourceManager;
	resourceManager->InitializeResources(renderer->GetDevice());

	physics = new Physics();
	physics->Initialize();

	RegisterScene(0, new IntroScene());
	RegisterScene(1, new GameOverScene());
	RegisterScene(2, new GameScene());
	RegisterScene(3, new WinScene());

	SwitchScene(0);
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
			auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTimePoint);
			float currentTime = static_cast<float>(elapsed.count() / 1000.0f);
			float deltaTime = currentTime - timeLastFrame;


			if (activeScene != nullptr)
			{
				if (activeScene->Quit())
				{
					Exit();
				}

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
				SwitchScene((int)activeScene->nextScene);
			}

			timeLastFrame = currentTime;
		}
	}

#if MULTITHREAD_PHYSICS
	fixedLoopThread.join();
#endif
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
	auto sceneIt = this->scenes.find(id);
	assert(sceneIt != scenes.end());

	if (activeScene == nullptr)
	{
		activeScene = (*sceneIt).second;
		activeScene->OnActivate();
	}
	else
	{
		if (activeScene != (*sceneIt).second)
		{
			previousScene = activeScene;
			activeScene = (*sceneIt).second;
			activeScene->OnActivate();
			previousScene->OnDeactivate();
		}
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
