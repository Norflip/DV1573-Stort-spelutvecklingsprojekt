#include "Engine.h"

Engine::Engine(Window& window) : window(window), camera(60.0f, window.GetWindowAspect())
{
	dxHandler.Initialize(window.GetHWND(), window.GetWidth(), window.GetHeight());
	renderer.Initialize(dxHandler);

	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.Compile(dxHandler.GetDevice());

	tmp_obj.SetMesh(ShittyOBJLoader::LoadOBJ("Models/cube.obj", dxHandler.GetDevice()));
	tmp_obj.SetMaterial(Material(shader));
	tmp_obj.GetTransform().SetPosition({ 0,0,5 });
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

			// UPDATE
			TMP_Update(deltaTime);

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

void Engine::TMP_Update(const float& deltaTime)
{
	renderer.BeginFrame();

	tmp_obj.GetTransform().Rotate(2.0f * deltaTime, 2.0f * deltaTime, 0.0f);
	tmp_obj.GetMaterial().BindToContext(dxHandler.GetContext());
	tmp_obj.Draw(&renderer, &camera);

	renderer.EndFrame();
}
