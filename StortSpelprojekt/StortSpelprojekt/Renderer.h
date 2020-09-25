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
	void SetBlendState(bool on);

	/* New stuff...  rendertoTexture is going to be in a post processing class later on */
	void RenderToTexture(Texture* texture, ID3D11Device* device, int width, int height);

	void SetRenderTarget(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv);
	void SetBackbufferRenderTarget();
	void ClearRenderTarget(ID3D11DeviceContext* context, ID3D11RenderTargetView* rtv, dx::XMFLOAT4 rgba);
	void Unbind();

	void Draw(const Mesh& mesh, const cb_Material& material, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMVECTOR cameraPosition);
	void DrawInstanced(const Mesh& mesh, const cb_Material& material, size_t count, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMVECTOR cameraPosition);
	void DrawSkeleton(const Mesh& mesh, const cb_Material& material, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection, cb_Skeleton& bones, dx::XMVECTOR cameraPosition);
	void DrawAlpha(const Mesh& mesh, const cb_Material& material, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMVECTOR cameraPosition);
	void DrawAlphaInstanced(const Mesh& mesh, const cb_Material& material, size_t count, dx::XMMATRIX view, dx::XMMATRIX projection, dx::XMVECTOR cameraPosition);

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

	cb_Skeleton cb_skeleton_data;
	ID3D11Buffer* skeleton_cbuffer;
	
	
	cb_Scene cb_scene;
	ID3D11Buffer* light_cbuffer;

	cb_Material cb_material_data;
	ID3D11Buffer* material_cbuffer;

	Window* outputWindow;

	/* Render to texture test - Is going to be in post processing class later etc. */
	ID3D11RenderTargetView* rtvTest;
	ID3D11Texture2D* renderTexture;
	ID3D11ShaderResourceView* srvTest;



	//blend states
	const float BLENDFACTOR[4] = {0};
	ID3D11BlendState* blendOff;
	ID3D11BlendState* blendOn;
	
};