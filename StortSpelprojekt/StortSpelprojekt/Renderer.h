#pragma once
#include "DXHelper.h"
#include "Mesh.h"
#include "Buffers.h"
#include "Texture.h"
namespace dx = DirectX;

class Renderer
{
	const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };

public:
	Renderer();
	virtual ~Renderer();

	void Initialize(Window* window);
	void BeginFrame();
	void EndFrame();
		
	/* New stuff...  rendertoTexture is going to be in a post processing class later on */
	void RenderToTexture(Texture* texture, ID3D11Device* device, int width, int height);

	void SetRenderTarget(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv);
	void SetBackbufferRenderTarget();
	void ClearRenderTarget(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, dx::XMFLOAT4 rgba);
	void Unbind();

	void Draw(const Mesh& mesh, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection);
	void DrawInstanced(const Mesh& mesh, size_t count, dx::XMMATRIX* models, dx::XMMATRIX view, dx::XMMATRIX projection);

	ID3D11Device* GetDevice() const { return this->device; }
	ID3D11DeviceContext* GetContext() const { return this->context; }
	Window* GetOutputWindow() const { return this->outputWindow; }

private:
	HRESULT hr;
	IDXGISwapChain* swapchain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	ID3D11RenderTargetView* backbuffer;
	ID3D11DepthStencilView* depthStencilView;

	cb_Object cb_object_data;
	ID3D11Buffer* obj_cbuffer;

	Window* outputWindow;

	/* Render to texture test - Is going to be in post processing class later etc. */
	ID3D11RenderTargetView* rtvTest;
	ID3D11Texture2D* renderTexture;
	ID3D11ShaderResourceView* srvTest;
};