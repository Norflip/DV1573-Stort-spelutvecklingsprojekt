#pragma once
#include "DXHelper.h"
#include "Mesh.h"
#include "Buffers.h"
namespace dx = DirectX;

class Renderer
{
	const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };

public:
	Renderer();
	virtual ~Renderer();

	void Initialize(Window& window);
	void BeginFrame();
	void EndFrame();
	
	void Draw (const Mesh& mesh, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection);
	void DrawInstanced(const Mesh& mesh, size_t count, dx::XMMATRIX* models, dx::XMMATRIX view, dx::XMMATRIX projection);

	ID3D11Device* GetDevice() const { return this->device; }
	ID3D11DeviceContext* GetContext() const { return this->context; }

private:
	IDXGISwapChain* swapchain;
	ID3D11RenderTargetView* backbuffer;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	cb_Object cb_object_data;
	ID3D11Buffer* obj_cbuffer;
};