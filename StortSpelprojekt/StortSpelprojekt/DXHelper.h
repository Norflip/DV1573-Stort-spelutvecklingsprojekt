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
	GEOMETRY_EXAMPLE = 1 << 2
};

DEFINE_ENUM_FLAG_OPERATORS(ShaderBindFlag);

struct CBuffer
{
	ID3D11Buffer* buffer;
	size_t slot;
	ShaderBindFlag flag;
};

namespace DXHelper
{
	void CreateSwapchain(const Window& window, ID3D11Device** device, ID3D11DeviceContext** context, IDXGISwapChain** swapchain, ID3D11RenderTargetView** backbuffer);
	
	void CreateCBuffer(ID3D11Device* device, ID3D11Buffer** buffer, void* initdata, unsigned int byteSize);
	void BindCBuffer(ID3D11DeviceContext* context, ID3D11Buffer* buffer, void* data, size_t slot, ShaderBindFlag flag);
}