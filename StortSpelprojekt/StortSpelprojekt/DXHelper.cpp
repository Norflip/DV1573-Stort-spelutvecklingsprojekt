#include "stdafx.h"
#include "DXHelper.h"

void DXHelper::CreateSwapchain(const Window& window, _Out_ ID3D11Device** device, _Out_ ID3D11DeviceContext** context, _Out_ IDXGISwapChain** swapchain,
	_Out_ IDXGIAdapter** adapter, _Out_ IDXGIFactory** factory, _Out_ IDXGIDevice** dxDevice, DXGI_MODE_DESC& currentModeDescription)
{
	size_t width = window.GetWidth();
	size_t height = window.GetHeight();

	UINT swapchainFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
//#ifdef  _DEBUG
//	swapchainFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif

	D3D_FEATURE_LEVEL featureLevel[] = { D3D_FEATURE_LEVEL_11_0 };

	HRESULT hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, 0, swapchainFlags, NULL, 0, D3D11_SDK_VERSION, device, featureLevel, context);

	assert(SUCCEEDED(hr));

	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = 0;
	scd.BufferDesc.Height = 0;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 2;
	scd.OutputWindow = window.GetHWND();
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	IDXGIOutput* output = nullptr;
	hr = (*device)->QueryInterface(dxDevice);
	assert(SUCCEEDED(hr));
	hr = (*dxDevice)->GetAdapter(adapter);
	assert(SUCCEEDED(hr));
	hr = (*adapter)->GetParent(__uuidof(IDXGIFactory), (void**)factory);
	assert(SUCCEEDED(hr));
	hr = (*factory)->CreateSwapChain(*device, &scd, swapchain);
	assert(SUCCEEDED(hr));
	ID3D11RasterizerState* rasterizerState = CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, *device);
	(*context)->RSSetState(rasterizerState);


	hr = (*swapchain)->GetContainingOutput(&output);
	assert(SUCCEEDED(hr));
	unsigned int numberOfSupportedModes;
	hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numberOfSupportedModes, NULL);
	assert(SUCCEEDED(hr));
	DXGI_MODE_DESC* supportedModes;
	supportedModes = new DXGI_MODE_DESC[numberOfSupportedModes];
	ZeroMemory(supportedModes, sizeof(DXGI_MODE_DESC) * numberOfSupportedModes);
	hr = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &numberOfSupportedModes, supportedModes);
	assert(SUCCEEDED(hr));
	output->Release();

	bool supportedMode = false;

	for (unsigned int i = 0; i < numberOfSupportedModes; i++)
	{
		if ((unsigned int)window.GetWidth() == supportedModes[i].Width && window.GetHeight() == supportedModes[i].Height)
		{
			supportedMode = true;
			currentModeDescription = supportedModes[i];
			break;
		}
	}
	if (!supportedMode)
	{

		currentModeDescription = supportedModes[0];
	}


	delete[] supportedModes;
}

void DXHelper::CreateBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff)
{
	HRESULT hr;
	D3D11_BLEND_DESC blendDescOn;
	ZeroMemory(&blendDescOn, sizeof(D3D11_BLEND_DESC));
	// HERE IS MY GREATEST SUCC
	blendDescOn.AlphaToCoverageEnable = TRUE;
	blendDescOn.IndependentBlendEnable = TRUE;
	blendDescOn.RenderTarget[0].BlendEnable = TRUE;
	blendDescOn.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescOn.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescOn.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescOn.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDescOn.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	D3D11_BLEND_DESC blendDescOff;
	ZeroMemory(&blendDescOff, sizeof(D3D11_BLEND_DESC));

	blendDescOff.RenderTarget[0].BlendEnable = false;
	blendDescOff.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	hr = device->CreateBlendState(&blendDescOn, blendOn);
	assert(SUCCEEDED(hr));

	hr = device->CreateBlendState(&blendDescOff, blendOff);
	assert(SUCCEEDED(hr));

	

}

void DXHelper::CreateParticleBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff)
{
	D3D11_BLEND_DESC blendStateDesc;
	ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.AlphaToCoverageEnable = true;
	blendStateDesc.IndependentBlendEnable = true;	 // can be true 
	blendStateDesc.RenderTarget[0].BlendEnable = true;
	blendStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	HRESULT hr = device->CreateBlendState(&blendStateDesc, blendOn);
	assert(SUCCEEDED(hr));

	//ZeroMemory(&blendStateDesc, sizeof(D3D11_BLEND_DESC));
	blendStateDesc.RenderTarget[0].BlendEnable = false;
	blendStateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateBlendState(&blendStateDesc, blendOff);
	assert(SUCCEEDED(hr));
}

void DXHelper::CreateRSState(ID3D11Device* device, ID3D11RasterizerState** cullBack, ID3D11RasterizerState** cullNone, ID3D11RasterizerState** CCWO)
{
	// DEFAULT RASTERIZER STATE
	D3D11_RASTERIZER_DESC rasterizerDescription;
	ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDescription.CullMode = D3D11_CULL_BACK;
	rasterizerDescription.FillMode = D3D11_FILL_SOLID;
	rasterizerDescription.DepthClipEnable = true;




	HRESULT resultCreateRasterizer = device->CreateRasterizerState(&rasterizerDescription, cullBack);
	assert(SUCCEEDED(resultCreateRasterizer));

	rasterizerDescription.CullMode = D3D11_CULL_NONE;

	resultCreateRasterizer = device->CreateRasterizerState(&rasterizerDescription, cullNone);
	assert(SUCCEEDED(resultCreateRasterizer));

	rasterizerDescription.CullMode = D3D11_CULL_BACK;

	rasterizerDescription.FrontCounterClockwise = true;

	resultCreateRasterizer = device->CreateRasterizerState(&rasterizerDescription, CCWO);
	assert(SUCCEEDED(resultCreateRasterizer));
}



RenderTexture DXHelper::CreateBackbuffer(size_t width, size_t height, ID3D11Device* device,  IDXGISwapChain* swapchain)
{

	RenderTexture rt;
	rt.width = width;
	rt.height = height;

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

	hr = (device)->CreateDepthStencilView(depthTexture, &dsvDesc, &rt.dsv);
	assert(SUCCEEDED(hr));

	depthTexture->Release();
	depthTexture = nullptr;

	ID3D11Texture2D* backBufferPtr;
	HRESULT getBackbufferResult = swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	assert(SUCCEEDED(getBackbufferResult));
	(device)->CreateRenderTargetView(backBufferPtr, nullptr, &rt.rtv);
	backBufferPtr->Release();

	ZeroMemory(&rt.viewport, sizeof(D3D11_VIEWPORT));
	rt.viewport.Width = static_cast<FLOAT>(width);
	rt.viewport.Height = static_cast<FLOAT>(height);
	rt.viewport.TopLeftX = 0;
	rt.viewport.TopLeftY = 0;
	rt.viewport.MinDepth = 0.0f;
	rt.viewport.MaxDepth = 1.0f;

	return rt;
}

RenderTexture DXHelper::CreateRenderTexture(size_t width, size_t height, ID3D11Device* device, ID3D11DeviceContext* context, ID3D11DepthStencilState** dss)
{
	RenderTexture rt;
	rt.width = width;
	rt.height = height;

	ZeroMemory(&rt.viewport, sizeof(D3D11_VIEWPORT));
	rt.viewport.TopLeftX = 0.0f;
	rt.viewport.TopLeftY = 0.0f;
	rt.viewport.Width = static_cast<float>(width);
	rt.viewport.Height = static_cast<float>(height);
	rt.viewport.MinDepth = 0.0f;
	rt.viewport.MaxDepth = 1.0f;

	ID3D11Texture2D* texture;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	HRESULT hr = device->CreateTexture2D(&textureDesc, NULL, &texture);
	assert(SUCCEEDED(hr));


	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	ZeroMemory(&renderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	renderTargetViewDesc.Format = textureDesc.Format;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;

	hr = device->CreateRenderTargetView(texture, &renderTargetViewDesc, &rt.rtv);
	assert(SUCCEEDED(hr));


	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	ZeroMemory(&shaderResourceViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	shaderResourceViewDesc.Format = textureDesc.Format;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = 1;
	hr = device->CreateShaderResourceView(texture, &shaderResourceViewDesc, &rt.srv);
	assert(SUCCEEDED(hr));

	texture->Release();
	texture = nullptr;


	// DEPTH STENCIL

	D3D11_DEPTH_STENCIL_DESC depthStencilStateDsc;
	ZeroMemory(&depthStencilStateDsc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDsc.DepthEnable = true;
	depthStencilStateDsc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDsc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	depthStencilStateDsc.StencilEnable = false;
	depthStencilStateDsc.StencilReadMask = 0xFF;
	depthStencilStateDsc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.s
	depthStencilStateDsc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilStateDsc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilStateDsc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilStateDsc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilStateDsc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	//ID3D11DepthStencilState* dss;
	HRESULT createDepthStencilResult = device->CreateDepthStencilState(&depthStencilStateDsc, dss);
	assert(SUCCEEDED(createDepthStencilResult));

	context->OMSetDepthStencilState(*dss, 1);


	ID3D11Texture2D* depthTex;

	// DEPTH TEXTURE
	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = width;
	depthTexDesc.Height = height;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 1;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;


	hr = device->CreateTexture2D(&depthTexDesc, 0, &depthTex);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //DXGI_FORMAT_D32_FLOAT;//DXGI_FORMAT_D24_UNORM_S8_UINT; // DXGI_FORMAT_D32_FLOAT;//DXGI_FORMAT_D24_UNORM_S8_UINT; // DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(depthTex, &dsvDesc, &rt.dsv);
	assert(SUCCEEDED(hr));


	// SHADER RESOURCE
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDescription;
	ZeroMemory(&shaderResourceDescription, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));

	shaderResourceDescription.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; ///DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // DXGI_FORMAT_R32_UINT;//depthTexDesc.Format;//DXGI_FORMAT_R32G32B32A32_FLOAT;// DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceDescription.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	//shaderResourceDescription.Texture2D.MostDetailedMip = 0;
	shaderResourceDescription.Texture2D.MipLevels = 1;

	HRESULT shaderResourceView = device->CreateShaderResourceView(depthTex, &shaderResourceDescription, &rt.depthSRV);


	depthTex->Release();
	depthTex = nullptr;




	return rt;
}

void DXHelper::CreateDepthStencilStates(ID3D11Device* device, ID3D11DepthStencilState** depthOn, ID3D11DepthStencilState** depthOff)
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;   // LESS OR LESS EQUAL

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;



	HRESULT hr = device->CreateDepthStencilState(&depthStencilDesc, depthOn);
	assert(SUCCEEDED(hr));


	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
	depthStencilDesc.DepthEnable = false;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	// Set up the description of the stencil state.
	depthStencilDesc.StencilEnable = false;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	hr = device->CreateDepthStencilState(&depthStencilDesc, depthOff);
	assert(SUCCEEDED(hr));
}

void DXHelper::CreateVertexBuffer(ID3D11Device* device, size_t verticeCount, size_t vertexSize, void* vertices, ID3D11Buffer** vertexBuffer)
{
	// creates vertex buffer
	D3D11_BUFFER_DESC vertexBufferDescription;
	ZeroMemory(&vertexBufferDescription, sizeof(vertexBufferDescription));
	vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDescription.ByteWidth = static_cast<unsigned int>(vertexSize * verticeCount);
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA vertexBuffer_subResource;
	ZeroMemory(&vertexBuffer_subResource, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBuffer_subResource.pSysMem = vertices;
	vertexBuffer_subResource.SysMemPitch = 0;
	vertexBuffer_subResource.SysMemSlicePitch = 0;

	HRESULT vertexBufferResult = device->CreateBuffer(&vertexBufferDescription, &vertexBuffer_subResource, vertexBuffer);
	assert(SUCCEEDED(vertexBufferResult));
}

void DXHelper::CreateIndexBuffer(ID3D11Device* device, size_t indexCount, unsigned int* indicies, ID3D11Buffer** indexBuffer)
{
	// creates index buffer
	D3D11_BUFFER_DESC indexBufferDescription;
	indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDescription.ByteWidth = (UINT)(sizeof(unsigned int) * indexCount);
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescription.CPUAccessFlags = 0;
	indexBufferDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBuffer_subResource;
	indexBuffer_subResource.pSysMem = indicies;
	indexBuffer_subResource.SysMemPitch = 0;
	indexBuffer_subResource.SysMemSlicePitch = 0;

	HRESULT indexBufferResult = device->CreateBuffer(&indexBufferDescription, &indexBuffer_subResource, indexBuffer);
	assert(SUCCEEDED(indexBufferResult));
}

ID3D11SamplerState* DXHelper::CreateSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, ID3D11Device* device)
{
	int hash = static_cast<int>(mode) ^ static_cast<int>(filter);
	auto findSampler = m_samplerCache.find(hash);

	if (findSampler == m_samplerCache.end())
	{
		D3D11_SAMPLER_DESC samplerDescription;
		ZeroMemory(&samplerDescription, sizeof(D3D11_SAMPLER_DESC));
		samplerDescription.Filter = filter;
		samplerDescription.AddressU = mode;
		samplerDescription.AddressV = mode;
		samplerDescription.AddressW = mode;
		samplerDescription.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		samplerDescription.MinLOD = 0.0f;
		samplerDescription.MaxLOD = D3D11_FLOAT32_MAX;
		samplerDescription.MipLODBias = 0.0f;
		samplerDescription.MaxAnisotropy = 0;

		for (size_t i = 0; i < 4; i++)
			samplerDescription.BorderColor[i] = 1.0f;

		ID3D11SamplerState* samplerState;
		ZeroMemory(&samplerState, sizeof(ID3D11SamplerState));
		device->CreateSamplerState(&samplerDescription, &samplerState);

		m_samplerCache.insert({ hash, samplerState });
	}

	return m_samplerCache[hash];
}

void DXHelper::CreateCopyBuffer(ID3D11Device* device, ID3D11Buffer** buffer, unsigned int byteStride, unsigned int arraySize)
{
	D3D11_BUFFER_DESC outputDesc = {};
	outputDesc.ByteWidth = byteStride * arraySize;
	outputDesc.StructureByteStride = byteStride;
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;

	HRESULT hr = (device->CreateBuffer(&outputDesc, 0, buffer));
	assert(SUCCEEDED(hr));
}

void DXHelper::CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* data, unsigned int byteStride, unsigned int arraySize, ID3D11UnorderedAccessView** uav)
{
	D3D11_BUFFER_DESC sBufferDesc = {};
	D3D11_SUBRESOURCE_DATA sBufferSub = {};

	sBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	sBufferDesc.ByteWidth = byteStride * arraySize; //sizeofStruct*nrOfElements
	sBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS; //D3D11_BIND_UNORDERED_ACCESS
	sBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;// 0; //D3D11_CPU_ACCESS_WRITE
	sBufferDesc.StructureByteStride = byteStride; //sizeofStruct
	sBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sBufferSub.pSysMem = data;

	HRESULT hr = device->CreateBuffer(&sBufferDesc, &sBufferSub, buffer);
	assert(SUCCEEDED(hr));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = arraySize;
	hr = device->CreateUnorderedAccessView(*buffer, &uavDesc, uav);

	assert(SUCCEEDED(hr));
}

void DXHelper::CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* data, unsigned int byteStride, unsigned int arraySize, ID3D11UnorderedAccessView** uav, ID3D11ShaderResourceView** srv)
{
	D3D11_BUFFER_DESC sBufferDesc = {};
	D3D11_SUBRESOURCE_DATA sBufferSub = {};

	sBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	sBufferDesc.ByteWidth = byteStride * arraySize; //sizeofStruct*nrOfElements
	sBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE; //D3D11_BIND_UNORDERED_ACCESS
	sBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;// 0; //D3D11_CPU_ACCESS_WRITE
	sBufferDesc.StructureByteStride = byteStride; //sizeofStruct
	sBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sBufferSub.pSysMem = data;

	HRESULT hr = device->CreateBuffer(&sBufferDesc, &sBufferSub, buffer);
	assert(SUCCEEDED(hr));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.Flags = 0;
	uavDesc.Buffer.NumElements = arraySize;
	hr = device->CreateUnorderedAccessView(*buffer, &uavDesc, uav);

	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = arraySize;
	hr = device->CreateShaderResourceView(*buffer, &srvDesc, srv);

	assert(SUCCEEDED(hr));

}

void DXHelper::CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* data, unsigned int byteStride, unsigned int arraySize, ID3D11ShaderResourceView** srv)
{
	D3D11_BUFFER_DESC sBufferDesc = {};
	D3D11_SUBRESOURCE_DATA sBufferSub = {};

	sBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	sBufferDesc.ByteWidth = byteStride * arraySize; //sizeofStruct*nrOfElements
	sBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	sBufferDesc.CPUAccessFlags = 0;// 0; //D3D11_CPU_ACCESS_WRITE
	sBufferDesc.StructureByteStride = byteStride; //sizeofStruct
	sBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	sBufferSub.pSysMem = data;

	HRESULT hr = device->CreateBuffer(&sBufferDesc, &sBufferSub, buffer);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = arraySize;
	hr = device->CreateShaderResourceView(*buffer, &srvDesc, srv);

	assert(SUCCEEDED(hr));
}

void DXHelper::BindStructuredBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag, ID3D11UnorderedAccessView** uav, const UINT* count)
{
	assert(buffer != 0);

	context->UpdateSubresource(buffer, 0, 0, data, 0, 0);

	int bflag = static_cast<int>(flag);

	if ((bflag & (int)ShaderBindFlag::COMPUTE) != 0)
		context->CSSetUnorderedAccessViews(slot, 1, uav, count);
}

void DXHelper::BindStructuredBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag, ID3D11ShaderResourceView** srv)
{
	assert(buffer != 0);

	context->UpdateSubresource(buffer, 0, 0, data, 0, 0);

	int bflag = static_cast<int>(flag);

	if ((bflag & (int)ShaderBindFlag::PIXEL) != 0)
		context->PSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
		context->VSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::HULL) != 0)
		context->HSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::DOMAINS) != 0)
		context->DSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
		context->GSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::COMPUTE) != 0)
		context->CSSetShaderResources(slot, 1, srv);
}

void DXHelper::BindStructuredBuffer(ID3D11DeviceContext* context, size_t slot, ShaderBindFlag flag, ID3D11ShaderResourceView** srv)
{
	int bflag = static_cast<int>(flag);

	if ((bflag & (int)ShaderBindFlag::PIXEL) != 0)
		context->PSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
		context->VSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::HULL) != 0)
		context->HSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::DOMAINS) != 0)
		context->DSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
		context->GSSetShaderResources(slot, 1, srv);

	if ((bflag & (int)ShaderBindFlag::COMPUTE) != 0)
		context->CSSetShaderResources(slot, 1, srv);
}




ID3D11RasterizerState* DXHelper::CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, ID3D11Device* device)
{
	// DEFAULT RASTERIZER STATE
	D3D11_RASTERIZER_DESC rasterizerDescription;
	ZeroMemory(&rasterizerDescription, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDescription.CullMode = cullMode;
	rasterizerDescription.FillMode = fillMode;
	rasterizerDescription.DepthClipEnable = true;

	ID3D11RasterizerState* rasterizerState;
	ZeroMemory(&rasterizerState, sizeof(ID3D11RasterizerState));

	HRESULT resultCreateRasterizer = device->CreateRasterizerState(&rasterizerDescription, &rasterizerState);
	assert(SUCCEEDED(resultCreateRasterizer));

	return rasterizerState;
}

void DXHelper::OnResize(IDXGISwapChain* swapchain, ID3D11DeviceContext* context, DXGI_MODE_DESC& currentModeDescription, BOOL& currentlyInFullscreen, const Window& window, ID3D11Device* device)
{
	HRESULT hr;
	DXGI_MODE_DESC zeroRefreshRate = currentModeDescription;
	zeroRefreshRate.RefreshRate.Numerator = 0;
	zeroRefreshRate.RefreshRate.Denominator = 0;

	BOOL inFullscreen = false;
	swapchain->GetFullscreenState(&inFullscreen, NULL);

	if (currentlyInFullscreen != inFullscreen)
	{
		if (inFullscreen)
		{
			hr = swapchain->ResizeTarget(&zeroRefreshRate);
			assert(SUCCEEDED(hr));
			hr = swapchain->SetFullscreenState(true, nullptr);
			assert(SUCCEEDED(hr));
		}
		else
		{
			hr = swapchain->SetFullscreenState(false, nullptr);
			assert(SUCCEEDED(hr));

			RECT rect = { 0, 0, (long)currentModeDescription.Width,  (long)currentModeDescription.Height };
			AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, WS_EX_OVERLAPPEDWINDOW);
			SetWindowPos(window.GetHWND(), HWND_TOP, 0, 0, rect.right - rect.left, rect.bottom - rect.top, SWP_NOMOVE);

		}
		currentlyInFullscreen = !currentlyInFullscreen;
	}
	hr = swapchain->ResizeTarget(&zeroRefreshRate);

	assert(SUCCEEDED(hr));
	UINT swapchainFlags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	context->ClearState();
	hr = swapchain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, swapchainFlags); //These are 0 so the default settings are used.

	assert(SUCCEEDED(hr));
}




void DXHelper::CreateInstanceBuffer(ID3D11Device* device, size_t instanceCount, size_t instanceDataSize, void* instanceData, ID3D11Buffer** instanceBuffer)
{
	assert(instanceCount != 0);

	D3D11_BUFFER_DESC instanceBufferDescription;
	ZeroMemory(&instanceBufferDescription, sizeof(D3D11_BUFFER_DESC));

	instanceBufferDescription.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDescription.ByteWidth = static_cast<unsigned int>(instanceDataSize * instanceCount);
	instanceBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDescription.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA instanceBuffer_subResource;
	ZeroMemory(&instanceBuffer_subResource, sizeof(D3D11_SUBRESOURCE_DATA));

	instanceBuffer_subResource.pSysMem = instanceData;
	HRESULT hr = device->CreateBuffer(&instanceBufferDescription, &instanceBuffer_subResource, instanceBuffer);

	assert(SUCCEEDED(hr));

}

