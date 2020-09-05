#include "Engine.h"

Engine::Engine(Window& window) : window(window), camera(60.0f, window.GetWindowAspect()), material(Shader())
{
	TMP_SetupDX11(window.GetHWND(), window.GetWidth(), window.GetHeight());
	mesh = ShittyOBJLoader::LoadOBJ("Models/cube.obj", device);

	Shader shader;
	shader.SetPixelShader(L"Shaders/Default_ps.hlsl");
	shader.SetVertexShader(L"Shaders/Default_vs.hlsl");
	shader.Compile(device);

	this->material = Material(shader);
	this->transform.position = { 0, 0, 5 };
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

void Engine::TMP_SetupDX11(HWND hwnd, size_t width, size_t height)
{
	DXGI_SWAP_CHAIN_DESC swapChainDescription;

	// Swapchain: swaps between two buffers
	ZeroMemory(&swapChainDescription, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDescription.BufferCount = 2;	 // one back buffer
	swapChainDescription.BufferDesc.Width = width;
	swapChainDescription.BufferDesc.Height = height;
	swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDescription.BufferDesc.RefreshRate.Numerator = 60; // sets framerate to 60 as max
	swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDescription.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDescription.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT; // DXGI_USAGE_SHADER_INPUT = can be used as a texture input in a hlsl file. 
	swapChainDescription.OutputWindow = hwnd;
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT swapflags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef  _DEBUG
	swapflags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };	// 
	HRESULT resultCreateDevAndSwap = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, swapflags, featureLevel, 1, D3D11_SDK_VERSION, &swapChainDescription, &swapchain, &device, nullptr, &context);
	assert(SUCCEEDED(resultCreateDevAndSwap));

	ID3D11Texture2D* backBufferPtr;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);

	if (backBufferPtr != nullptr)
	{
		device->CreateRenderTargetView(backBufferPtr, nullptr, &backbufferRTV);
		context->OMSetRenderTargets(1, &backbufferRTV, nullptr);
		backBufferPtr->Release();
	}

	// setup för viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	context->RSSetViewports(1, &viewport);


	// WORLD BUFFER
	static_assert(sizeof(WorldData) % 16 == 0, "struct is wrong size");
	ZeroMemory(&worldBuffer_ptr, sizeof(ID3D11Buffer));

	HRESULT hr;
	D3D11_BUFFER_DESC bufferDescription;
	D3D11_SUBRESOURCE_DATA subresourceData;
	ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDescription.CPUAccessFlags = 0;

	// WORLD BUFFER
	bufferDescription.ByteWidth = sizeof(cb_world);
	subresourceData.pSysMem = &cb_world;
	hr = device->CreateBuffer(&bufferDescription, &subresourceData, &worldBuffer_ptr);
	assert(SUCCEEDED(hr));
}

void Engine::TMP_Update(const float& deltaTime)
{
	Log::Add(std::to_string(deltaTime));

	const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };
	context->ClearRenderTargetView(backbufferRTV, DEFAULT_BG_COLOR);

	transform.Rotate(2.0f * deltaTime, 2.0f * deltaTime, 0.0f);

	material.BindToContext(context);
	TMP_DrawMesh(mesh, transform, camera);

	swapchain->Present(0, 0);
}

void Engine::TMP_DrawMesh(const Mesh& mesh, const Transform& transform, const Camera& camera)
{
	dx::XMMATRIX world = transform.GetWorldMatrix();
	cb_world.mvp = DirectX::XMMatrixTranspose(DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(world, camera.GetViewMatrix()), camera.GetProjectionMatrix()));
	cb_world.world = DirectX::XMMatrixTranspose(world);

	context->UpdateSubresource(worldBuffer_ptr, 0, 0, &cb_world, 0, 0);
	context->VSSetConstantBuffers(0, 1, &worldBuffer_ptr);

	UINT stride = sizeof(Mesh::Vertex);
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &mesh.vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(mesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->DrawIndexed(mesh.indices.size(), 0, 0);
}
