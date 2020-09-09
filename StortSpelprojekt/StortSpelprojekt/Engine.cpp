#include "Engine.h"

Engine::Engine(Window& window) : window(window)
{
	dxHandler.Initialize(window.GetHWND(), window.GetWidth(), window.GetHeight());
	renderer.Initialize(dxHandler);

	activeScene = new Scene();
	activeScene->Initialize(&dxHandler, &renderer);
}

Engine::~Engine()
{
}

void Engine::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	auto startTimePoint = std::chrono::high_resolution_clock::now();
	float timeLastFrame = 0.0f;
	float fixedTimeAccumulation = 0.0f;
	const float TARGET_FIXED_DELTA = 1.0f / 50.0f;
	
	bool running = true;

	while (running)
	{
		// Redirects the messages to the static WindowProc in Window.cpp
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
				running = false;
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

			fixedTimeAccumulation += deltaTime;
			while (fixedTimeAccumulation >= TARGET_FIXED_DELTA)
			{
				// FIXED UPDATE
				fixedTimeAccumulation -= TARGET_FIXED_DELTA;
			}

			timeLastFrame = currentTime;
		}
	}
}