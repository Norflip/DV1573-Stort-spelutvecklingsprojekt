#include "Engine.h"

Engine::Engine(Window& window) : window(window), activeScene(nullptr)
{
	dxHandler.Initialize(window);
	renderer.Initialize(dxHandler);

	RegisterScene(0, new Scene());
	SwitchScene(0);
}

Engine::~Engine()
{
}

void Engine::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	fixedLoopThread = std::thread (FixedUpdateLoop, this);
	
	auto startTimePoint = std::chrono::high_resolution_clock::now();
	float timeLastFrame = 0.0f;
	this->running = true;

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
				activeScene->Update(deltaTime);
				activeScene->Render();
			}

			timeLastFrame = currentTime;
		}
	}
}

void Engine::Exit()
{
	fixedLoopThread.join();
	running = false;
}

void Engine::RegisterScene(size_t id, Scene* scene)
{
	auto sceneIt = this->scenes.find(id);
	assert(sceneIt == scenes.end(), "Conflicting scene IDs");

	scene->Initialize(&dxHandler, &renderer);
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

	if (activeScene != nullptr)
		activeScene->OnDeactivate();

	activeScene = (*sceneIt).second;
	activeScene->OnActivate();
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

		// move to a seperate thread? 
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
