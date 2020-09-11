#include "DXHandler.h"

DXHandler::DXHandler() : context(nullptr), device(nullptr), swapchain(nullptr), backbuffer(nullptr)
{
}

DXHandler::~DXHandler()
{

	//context->Release();
	//device->Release();
	//swapchain->Release();
	//backbuffer->Release();
}

void DXHandler::Initialize(Window& window)
{
	DXGI_SWAP_CHAIN_DESC swapChainDescription;

	size_t width = window.GetWidth();
	size_t height = window.GetHeight();

	UINT swapchainFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef  _DEBUG
	swapchainFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
	swapChainDescription.OutputWindow = window.GetHWND();
	swapChainDescription.SampleDesc.Count = 1;
	swapChainDescription.SampleDesc.Quality = 0;
	swapChainDescription.Windowed = TRUE;
	swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };	// 
	HRESULT resultCreateDevAndSwap = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, swapchainFlags, featureLevel, 1, D3D11_SDK_VERSION, &swapChainDescription, &swapchain, &device, nullptr, &context);
	assert(SUCCEEDED(resultCreateDevAndSwap));

	ID3D11Texture2D* backBufferPtr;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);

	if (backBufferPtr != nullptr)
	{
		device->CreateRenderTargetView(backBufferPtr, nullptr, &backbuffer);
		context->OMSetRenderTargets(1, &backbuffer, nullptr);
		backBufferPtr->Release();
	}


	// BACKBUFFER VIEWPORT
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(width);
	viewport.Height = static_cast<FLOAT>(height);
	context->RSSetViewports(1, &viewport);


	// RASTERIZER STATE
	D3D11_RASTERIZER_DESC rasterizerDescription;
	ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDescription.CullMode = D3D11_CULL_BACK;
	rasterizerDescription.FillMode = D3D11_FILL_SOLID; //if we want wireframe, fill etc
	rasterizerDescription.DepthClipEnable = true;

	ID3D11RasterizerState* rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(ID3D11RasterizerState));

	HRESULT resultCreateRasterizer = device->CreateRasterizerState(&rasterizerDescription, &rasterizerState);
	assert(SUCCEEDED(resultCreateRasterizer));
	context->RSSetState(rasterizerState);
}
