#pragma once
#include <d3d11_1.h>
#include <dxgi.h> 
#include <d3dcompiler.h>
#include <assert.h>
#include <unordered_map>

#pragma comment(lib, "gdi32")
#pragma comment(lib, "d3d11") 
#pragma comment( lib, "dxgi" )   
#pragma comment(lib, "d3dcompiler.lib")

#include "Window.h"

enum class ShaderBindFlag
{
	NONE = 0,
	VERTEX = 1 << 0,
	PIXEL = 1 << 1,
	GEOMETRY = 1 << 2
};

DEFINE_ENUM_FLAG_OPERATORS(ShaderBindFlag);

struct RenderTexture
{
	size_t width, height;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;
	ID3D11DepthStencilView* dsv;
	D3D11_VIEWPORT viewport;

	RenderTexture() : rtv(nullptr), srv(nullptr), dsv(nullptr), width(-1), height(-1) {}
	void Release () 
	{
		if (rtv) rtv->Release();
		if (srv) srv->Release();
		if (dsv) dsv->Release();
	}
};







namespace DXHelper
{
	static std::unordered_map<int, ID3D11SamplerState*> m_samplerCache;

	void CreateSwapchain(const Window& window, ID3D11Device** device, ID3D11DeviceContext** context, IDXGISwapChain** swapchain);
	void CreateConstBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* initdata, unsigned int byteSize);
	void BindConstBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag);

	D3D11_MAPPED_SUBRESOURCE& BindInstanceBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer);

	void UnBindInstanceBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer);

	RenderTexture CreateBackbuffer(size_t width, size_t height, ID3D11Device* device, IDXGISwapChain* swapchain);
	RenderTexture CreateRenderTexture(size_t width, size_t height, ID3D11Device* device);
	void CreateBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff);
	void CreateRSState(ID3D11Device* device, ID3D11RasterizerState** cullBack, ID3D11RasterizerState** cullNone); //can add more in the future.

	//void CreateBackbuffer(size_t width, size_t height, ID3D11Device* device, IDXGISwapChain* swapchain, ID3D11RenderTargetView** backbuffer, ID3D11DepthStencilView** depthStencilView);

	void CreateVertexBuffer(ID3D11Device* device, size_t verticeCount, size_t vertexSize, void* vertices, ID3D11Buffer** vertexBuffer);
	void CreateIndexBuffer(ID3D11Device* device, size_t indexCount, unsigned int* indicies, ID3D11Buffer** indexBuffer);
	void CreateInstanceBuffer(ID3D11Device* device, size_t instanceCount, size_t instanceDataSize, void* instanceData, ID3D11Buffer** instanceBuffer);


	ID3D11ShaderResourceView* CreateTexture(unsigned char* buffer, size_t width, size_t height, size_t channels, DXGI_FORMAT format, ID3D11Device* device);
	ID3D11SamplerState* CreateSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, ID3D11Device* device);
}