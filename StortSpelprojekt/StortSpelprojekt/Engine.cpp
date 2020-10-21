#include "Engine.h"

Engine::Engine(HINSTANCE hInstance) : window(hInstance), activeScene(nullptr)
{
	Log::Open();

	window.Open(1600, 900);
	renderer = new Renderer();
	renderer->Initialize(&window);

	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));		

	resourceManager = new ResourceManager;
	resourceManager->InitializeResources(renderer->GetDevice());

	RegisterScene(0, new IntroScene(resourceManager));
	RegisterScene(1, new GameOverScene(resourceManager));
	RegisterScene(2, new GameScene(resourceManager));
	SwitchScene(0);
}

Engine::~Engine()
{
	for (int i = 0; i < scenes.size(); i++)
	{
		if (scenes[i])
		{
			delete scenes[i];
		}
	}
}

void Engine::Run()
{
	this->running = true;
	std::thread fixedLoopThread (Engine::FixedUpdateLoop, this);

	auto startTimePoint = std::chrono::high_resolution_clock::now();
	float timeLastFrame = 0.0f;

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
			
			if (activeScene != nullptr)
			{
				if (activeScene->Quit())
				{
					Exit();
				}
				float deltaTime = currentTime - timeLastFrame;

				activeScene->Update(deltaTime);
				activeScene->Render();

				SwitchScene((int)activeScene->nextScene);

			}

			timeLastFrame = currentTime;
		}
	}

	fixedLoopThread.join();
}

void Engine::Exit()
{
	running = false;
}

void Engine::RegisterScene(size_t id, Scene* scene)
{
	auto sceneIt = this->scenes.find(id);
	assert(sceneIt == scenes.end(), "Conflicting scene IDs");

	scene->Initialize(renderer);
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
	assert(sceneIt != scenes.end(), "No scene with ID " + std::to_string(id));

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
