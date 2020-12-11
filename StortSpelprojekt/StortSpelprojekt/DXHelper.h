#pragma once
#include <d3d11_1.h>
#include <dxgi.h> 
#include <d3dcompiler.h>
#include <assert.h>
#include <unordered_map>
#include <DirectXMath.h>
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
	GEOMETRY = 1 << 2,
	HULL = 1 << 3,
	DOMAINS = 1 << 4,
	COMPUTE = 1 << 5,
	SOGEOMETRY = 1 << 6,
};

DEFINE_ENUM_FLAG_OPERATORS(ShaderBindFlag);

struct RenderTexture
{
	size_t width, height;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;

	ID3D11DepthStencilView* dsv;
	ID3D11ShaderResourceView* depthSRV;

	D3D11_VIEWPORT viewport;

	RenderTexture() : rtv(nullptr), srv(nullptr), dsv(nullptr), depthSRV(nullptr), width(-1), height(-1) {}
	void Release () 
	{
		RELEASE(rtv);
		RELEASE(srv);
		RELEASE(dsv);
		RELEASE(depthSRV);
	}
};


namespace DXHelper
{
	static std::unordered_map<int, ID3D11SamplerState*> m_samplerCache;
	
	void CreateSwapchain(const Window& window, ID3D11Device** device, ID3D11DeviceContext** context, IDXGISwapChain** swapchain);
	RenderTexture CreateBackbuffer(size_t width, size_t height, ID3D11Device* device, IDXGISwapChain* swapchain);
	
	RenderTexture CreateRenderTexture(size_t width, size_t height, ID3D11Device* device, ID3D11DepthStencilState** dss);
	void CreateBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff);

	void CreateRSState(ID3D11Device* device, ID3D11RasterizerState** cullBack, ID3D11RasterizerState** cullNone, ID3D11RasterizerState** CCWO); //can add more in the future.

	//void CreateBackbuffer(size_t width, size_t height, ID3D11Device* device, IDXGISwapChain* swapchain, ID3D11RenderTargetView** backbuffer, ID3D11DepthStencilView** depthStencilView);

	/* Jadda stuff */
	void CreateParticleBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff);
	void CreateDepthStencilStates(ID3D11Device* device, ID3D11DepthStencilState** depthOn, ID3D11DepthStencilState** depthOff);

	void CreateVertexBuffer(ID3D11Device* device, size_t verticeCount, size_t vertexSize, void* vertices, ID3D11Buffer** vertexBuffer);
	void CreateIndexBuffer(ID3D11Device* device, size_t indexCount, unsigned int* indicies, ID3D11Buffer** indexBuffer);
	void CreateInstanceBuffer(ID3D11Device* device, size_t instanceCount, size_t instanceDataSize, void* instanceData, ID3D11Buffer** instanceBuffer);

	ID3D11SamplerState* CreateSampler(D3D11_FILTER filter, D3D11_TEXTURE_ADDRESS_MODE mode, ID3D11Device* device);

	//Structured buffer
	//stride is sizeof struct, width is all it contains.
	void CreateCopyBuffer(ID3D11Device* device, ID3D11Buffer** buffer, unsigned int byteStride, unsigned int arraySize);
	void CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* data, unsigned int byteStride, unsigned int arraySize, ID3D11UnorderedAccessView** uav);
	void CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* data, unsigned int byteStride, unsigned int arraySize, ID3D11UnorderedAccessView** uav, ID3D11ShaderResourceView** srv);
	void CreateStructuredBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* data, unsigned int byteStride, unsigned int arraySize, ID3D11ShaderResourceView** srv);
	void BindStructuredBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag, ID3D11UnorderedAccessView** uav, const UINT* count);
	void BindStructuredBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag, ID3D11ShaderResourceView** srv);
	void BindStructuredBuffer(ID3D11DeviceContext* context, size_t slot, ShaderBindFlag flag, ID3D11ShaderResourceView** srv);
	ID3D11RasterizerState* CreateRasterizerState(D3D11_CULL_MODE cullMode, D3D11_FILL_MODE fillMode, ID3D11Device* device);
	

}