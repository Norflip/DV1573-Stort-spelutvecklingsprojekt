#pragma once
#include <queue>
#include <unordered_map>

#include "DXHelper.h"
#include "Mesh.h"
#include "Buffers.h"
#include "Texture.h"
#include "Material.h"
#include "CameraComponent.h"
namespace dx = DirectX;

class RenderPass;

class Renderer
{
	const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };

	struct RenderItem
	{
		Mesh mesh;
		Material material;

		bool instanced;
		size_t instanceCount;

		dx::XMMATRIX world;
		const CameraComponent* camera;
	};

public:
	Renderer();
	virtual ~Renderer();

	void Initialize(Window* window);
	void BeginFrame();
	void EndFrame();
		
	/* New stuff...  rendertoTexture is going to be in a post processing class later on */
	void RenderToTexture(Texture* texture, ID3D11Device* device, int width, int height);

	void SetRenderTarget(ID3D11RenderTargetView* rtv);
	void SetBackbufferRenderTarget();
	void ClearRenderTarget(ID3D11RenderTargetView* rtv, dx::XMFLOAT4 rgba);
	
	void DrawItemsToTarget();
	void Unbind();

	void AddRenderPass(RenderPass*);
	void RemoveRenderPass(RenderPass*);

	void Draw(const Mesh& mesh, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera);
	void DrawInstanced(const Mesh& mesh, size_t count, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera);
	void DrawSkeleton(const Mesh& mesh, dx::XMMATRIX model, dx::XMMATRIX view, dx::XMMATRIX projection, cb_Skeleton& bones);

	ID3D11Device* GetDevice() const { return this->device; }
	ID3D11DeviceContext* GetContext() const { return this->context; }
	Window* GetOutputWindow() const { return this->outputWindow; }

private:
	void m_Draw(const RenderItem& item);
	void m_DrawInstanced(const RenderItem& item);

private:
	IDXGISwapChain* swapchain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	ID3D11RenderTargetView* backbuffer;
	ID3D11DepthStencilView* depthStencilView;

	cb_Object cb_object_data;
	ID3D11Buffer* obj_cbuffer;

	cb_Skeleton cb_skeleton_data;
	ID3D11Buffer* skeleton_cbuffer;
	
	//måste avallokeras!!!
	cb_Scene cb_scene;
	ID3D11Buffer* light_cbuffer;

	cb_Material cb_material_data;
	ID3D11Buffer* material_cbuffer;

	Window* outputWindow;

	/* Render to texture test - Is going to be in post processing class later etc. */
	ID3D11RenderTargetView* rtvTest;
	ID3D11Texture2D* renderTexture;
	ID3D11ShaderResourceView* srvTest;

	std::unordered_map<size_t, std::queue<RenderItem>> itemQueue;
};