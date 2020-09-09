#include "Engine.h"

Engine::Engine(Window& window) : window(window), camera(60.0f, window.GetWindowAspect()), material(Shader())
{
	dxHandler.Initialize(window.GetHWND(), window.GetWidth(), window.GetHeight());
	objectBuffer.Initialize(0, dxHandler.GetDevice());

	mesh = ShittyOBJLoader::LoadOBJ("Models/cube.obj", dxHandler.GetDevice());

	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.Compile(dxHandler.GetDevice());

	this->material = Material(shader);
	this->transform.SetPosition({ 0, 0, 5 });
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
	Log::Add(std::to_string(deltaTime));

	const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };
	dxHandler.GetContext()->ClearRenderTargetView(dxHandler.GetBackbuffer(), DEFAULT_BG_COLOR);

	transform.Rotate(2.0f * deltaTime, 2.0f * deltaTime, 0.0f);
	material.BindToContext(dxHandler.GetContext());
	TMP_DrawMesh(mesh, transform, camera);

	dxHandler.GetSwapchain()->Present(0, 0);
}

void Engine::TMP_DrawMesh(const Mesh& mesh, const Transform& transform, const Camera& camera)
{
	auto cb_objectData = objectBuffer.GetData();

	dx::XMMATRIX world = transform.GetWorldMatrix();
	cb_objectData->mvp = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(world, camera.GetViewMatrix()), camera.GetProjectionMatrix()));
	cb_objectData->world = DirectX::XMMatrixTranspose(world);
	objectBuffer.Bind(dxHandler.GetContext());

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	dxHandler.GetContext()->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	dxHandler.GetContext()->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	dxHandler.GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//context->DrawIndexed(mesh.indices.size(), 0, 0);

	dxHandler.GetContext()->DrawIndexedInstanced(mesh.indices.size(), 10, 0, 0, 0);
}
