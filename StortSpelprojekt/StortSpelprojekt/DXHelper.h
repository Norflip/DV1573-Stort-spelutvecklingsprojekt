#pragma once
#include <d3d11_1.h>
#include <dxgi.h> 
#include <d3dcompiler.h>
#include <assert.h>

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
	/*SKELETON = 1 << 3,
	INSTANCE = 1 << 4,
	ALPHA = 1 << 5*/

};

DEFINE_ENUM_FLAG_OPERATORS(ShaderBindFlag);

namespace DXHelper
{
	void CreateSwapchain(const Window& window, ID3D11Device** device, ID3D11DeviceContext** context, IDXGISwapChain** swapchain);
	void CreateConstBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* initdata, unsigned int byteSize);	
	void BindConstBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag);

	void CreateBlendState(ID3D11Device* device, ID3D11BlendState** blendOn, ID3D11BlendState** blendOff, ID3D11DepthStencilState** depthStencilState);

	void CreateBackbuffer(size_t width, size_t height, ID3D11Device* device, IDXGISwapChain* swapchain, ID3D11RenderTargetView** backbuffer, ID3D11DepthStencilView** depthStencilView);

	void CreateVertexBuffer(ID3D11Device* device, size_t verticeCount, size_t vertexSize, void* vertices, ID3D11Buffer** vertexBuffer);
	void CreateIndexBuffer(ID3D11Device* device, size_t indexCount, unsigned int* indicies, ID3D11Buffer** indexBuffer);
	void CreateInstanceBuffer(ID3D11Device* device, size_t instanceCount, size_t instanceDataSize, void* instanceData, ID3D11Buffer** instanceBuffer);
}