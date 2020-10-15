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

	swapChainDescription.BufferCount = 2;	 // one back buffer???
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

	ID3D11RasterizerState* rasterizerState = CreateRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID, *device);
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
	/*ID3D11Buffer* buff = NULL;
	context->PSSetConstantBuffers(slot, 1, &buff);
	context->VSSetConstantBuffers(slot, 1, &buff);
	context->GSSetConstantBuffers(slot, 1, &buff);*/

	context->UpdateSubresource(buffer, 0, 0, data, 0, 0);

	int bflag = static_cast<int>(flag);

	if ((bflag & (int)ShaderBindFlag::PIXEL) != 0)
		context->PSSetConstantBuffers(slot, 1, &buffer);
	
	if ((bflag & (int)ShaderBindFlag::VERTEX) != 0)
		context->VSSetConstantBuffers(slot, 1, &buffer);

	if ((bflag & (int)ShaderBindFlag::GEOMETRY) != 0)
		context->GSSetConstantBuffers(slot, 1, &buffer);

	if ((bflag & (int)ShaderBindFlag::HULL) != 0)
		context->HSSetConstantBuffers(slot, 1, &buffer);

	if ((bflag & (int)ShaderBindFlag::DOMAINS) != 0)
		context->DSSetConstantBuffers(slot, 1, &buffer);


}

D3D11_MAPPED_SUBRESOURCE& DXHelper::BindInstanceBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedData = {}; //zeroing out the memory like this.
	

	
	context->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);

	return mappedData;

	


}

void DXHelper::UnBindInstanceBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer)
{
	context->Unmap(buffer, 0);
}

void DXHelper::CreateBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff)
{
	HRESULT hr;
	D3D11_BLEND_DESC blendDescOn;
	ZeroMemory(&blendDescOn, sizeof(D3D11_BLEND_DESC));



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
	depthTexDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = 0;


	hr = device->CreateTexture2D(&depthTexDesc, 0, &depthTex);
	assert(SUCCEEDED(hr));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = depthTexDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	hr = device->CreateDepthStencilView(depthTex, &dsvDesc, &rt.dsv);
	assert(SUCCEEDED(hr));

	depthTex->Release();
	depthTex = nullptr;

	return rt;
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

ID3D11ShaderResourceView* DXHelper::CreateTexture(unsigned char* buffer, size_t width, size_t height, size_t channels, DXGI_FORMAT format, ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureResourceData;
	ZeroMemory(&textureResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	textureResourceData.pSysMem = buffer;
	textureResourceData.SysMemPitch = width * 4;


	ID3D11Texture2D* texture;
	ZeroMemory(&texture, sizeof(ID3D11Texture2D));
	device->CreateTexture2D(&textureDesc, &textureResourceData, &texture);

	ID3D11ShaderResourceView* srv = nullptr;

	if (texture != nullptr)
	{
		ZeroMemory(&srv, sizeof(ID3D11ShaderResourceView));
		device->CreateShaderResourceView(texture, nullptr, &srv);

		texture->Release();
		texture = nullptr;
	}

	return srv;
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

