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

enum class ShaderTypeFlag
{
	NONE = 0,
	VERTEX = 1 << 0,
	PIXEL = 1 << 1,
	GEOMETRY_EXAMPLE = 1 << 2
};

DEFINE_ENUM_FLAG_OPERATORS(ShaderTypeFlag);

class DXHandler
{
public:
	DXHandler();
	virtual ~DXHandler();

	void Initialize(Window& window);
	
	ID3D11DeviceContext*	GetContext()	const { return this->context; }
	ID3D11Device*			GetDevice()		const { return this->device; }
	IDXGISwapChain*			GetSwapchain()	const { return this->swapchain; }
	ID3D11RenderTargetView* GetBackbuffer() const { return this->backbuffer; }

private:
	ID3D11RenderTargetView* backbuffer;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapchain;

};