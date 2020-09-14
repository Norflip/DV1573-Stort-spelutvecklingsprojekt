#include "DXHelper.h"

void DXHelper::CreateSwapchain(const Window& window, _Out_ ID3D11Device** device, _Out_ ID3D11DeviceContext** context, _Out_ IDXGISwapChain** swapchain)
{

	size_t width = window.GetWidth();
	size_t height = window.GetHeight();

	UINT swapchainFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef  _DEBUG
	swapchainFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	// Swapchain: swaps between two buffers
	DXGI_SWAP_CHAIN_DESC swapChainDescription;
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

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };
	HRESULT resultCreateDevAndSwap = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, swapchainFlags, featureLevel, 1, D3D11_SDK_VERSION, &swapChainDescription, swapchain, device, nullptr, context);
	assert(SUCCEEDED(resultCreateDevAndSwap));


	// DEFAULT RASTERIZER STATE
	D3D11_RASTERIZER_DESC rasterizerDescription;
	ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDescription.CullMode = D3D11_CULL_NONE;
	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.DepthClipEnable = true;

	ID3D11RasterizerState* rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(ID3D11RasterizerState));

	HRESULT resultCreateRasterizer = (*device)->CreateRasterizerState(&rasterizerDescription, &rasterizerState);
	assert(SUCCEEDED(resultCreateRasterizer));
	(*context)->RSSetState(rasterizerState);



}

void DXHelper::CreateConstBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* initdata, unsigned int byteSize)
{
	D3D11_BUFFER_DESC bufferDescription;
	ZeroMemory(&bufferDescription, sizeof(D3D11_BUFFER_DESC));
	bufferDescription.Usage = D3D11_USAGE_DEFAULT;
	bufferDescription.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	D3D11_SUBRESOURCE_DATA subresourceData;
	ZeroMemory(&subresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferDescription.ByteWidth = byteSize;
	subresourceData.pSysMem = &initdata;

	HRESULT createdBufferResult = device->CreateBuffer(&bufferDescription, &subresourceData, buffer);
	assert(SUCCEEDED(createdBufferResult));
}

void DXHelper::BindConstBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag)
{
	context->UpdateSubresource(buffer, 0, 0, data, 0, 0);

	int bflag = static_cast<int>(flag);

	if ((bflag & (int)ShaderBindFlag::PIXEL) != 0)
		context->PSSetConstantBuffers(slot, 1, &buffer);
	
	if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
		context->VSSetConstantBuffers(slot, 1, &buffer);

	if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
		context->GSSetConstantBuffers(slot, 1, &buffer);
}

void DXHelper::CreateBackbuffer(size_t width, size_t height, ID3D11Device* device,  IDXGISwapChain* swapchain, ID3D11RenderTargetView** backbuffer, ID3D11DepthStencilView** depthStencilView)
{
	HRESULT hr;

	// DEPTH TEXTURE
	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = width;
	depthTexDesc.Height = height;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;

	ID3D11Texture2D* depthTexture;
	hr = (device)->CreateTexture2D(&depthTexDesc, 0, &depthTexture);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = (device)->CreateDepthStencilView(depthTexture, &dsvDesc, depthStencilView);
	assert(SUCCEEDED(hr));

	depthTexture->Release();
	depthTexture = nullptr;


	ID3D11Texture2D* backBufferPtr;
	HRESULT getBackbufferResult = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	assert(SUCCEEDED(getBackbufferResult));

	(device)->CreateRenderTargetView(backBufferPtr, nullptr, backbuffer);
	backBufferPtr->Release();
}
