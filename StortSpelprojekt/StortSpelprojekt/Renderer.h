#pragma once
#include <queue>
#include <unordered_map>

#include "DXHelper.h"
#include "Mesh.h"
#include "Buffers.h"
#include "Texture.h"
#include "Material.h"
#include "CameraComponent.h"
#include <time.h>

namespace dx = DirectX;

class RenderPass;



class Renderer
{
	const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };

	struct RenderItem
	{
		enum class Type
		{
			Default,
			Instanced,
			Skeleton,
			Grass
		};

		const Mesh* mesh;
		const Material* material;

		Type type;
		size_t instanceCount;
		
		std::vector<dx::XMFLOAT4X4>* bones;
		dx::XMMATRIX world;
		const CameraComponent* camera;
	};

	typedef std::unordered_map<size_t, std::queue<RenderItem>> RenderQueue;

public:
	Renderer();
	virtual ~Renderer();

	void Initialize(Window* window);
	
	void BeginManualRenderPass(RenderTexture& target);
	void EndManualRenderPass();

	void DrawQueueToTarget(RenderQueue& queue);
	void RenderFrame();
	
	void AddRenderPass(RenderPass*);

	void Draw(const Mesh& mesh, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera);
	void DrawInstanced(const Mesh& mesh, const size_t& count, const Material& material, const CameraComponent& camera);
	void DrawSkeleton(const Mesh& mesh, const Material& material, const dx::XMMATRIX& model, const CameraComponent& camera, std::vector<dx::XMFLOAT4X4>& bones);
	void DrawGrass(const CameraComponent& camera, const Mesh& mesh, const Material& material, const dx::XMMATRIX& model);
	
	void SetRSToCullNone(bool);

	ID3D11Device* GetDevice() const { return this->device; }
	ID3D11DeviceContext* GetContext() const { return this->context; }
	Window* GetOutputWindow() const { return this->outputWindow; }

	void DrawScreenQuad(const Material& Material);

	void ClearRenderTarget(const RenderTexture& target);
	void SetRenderTarget(const RenderTexture& target);

	void UpdateTime(float time);
	bool IsDrawingShapes() const { return this->drawShapes; }
	void DrawShaped(bool draw) { this->drawShapes = draw; }

private:
	void AddItem(const RenderItem& item, bool transparent);
	void DrawRenderItem(const RenderItem& item);
	void DrawRenderItemInstanced(const RenderItem& item);
	void DrawRenderItemSkeleton(const RenderItem& item);
	void DrawRenderItemGrass(const RenderItem& item);
	
	
private:
	IDXGISwapChain* swapchain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;

	RenderTexture backbuffer;
	RenderTexture midbuffers [2];
	
	Material screenQuadMaterial;
	Mesh screenQuadMesh;

	cb_Object cb_object_data;
	ID3D11Buffer* obj_cbuffer;

	std::vector<dx::XMFLOAT4X4> srv_skeleton_data;
	ID3D11Buffer* skeleton_srvbuffer;
	ID3D11ShaderResourceView* skeleton_srv;
	
	cb_Scene cb_scene;
	ID3D11Buffer* light_cbuffer;

	cb_Material cb_material_data;
	ID3D11Buffer* material_cbuffer;

	Window* outputWindow;

	RenderQueue opaqueItemQueue;
	RenderQueue transparentItemQueue;
	std::vector<RenderPass*> passes;

	//blendstate
	ID3D11BlendState* blendStateOn;
	ID3D11BlendState* blendStateOff;

	const float BLENDSTATEMASK[4] = { 0.0f };

	bool drawShapes = true;
  
	//rasterizer
	ID3D11RasterizerState* rasterizerStateCullBack;
	ID3D11RasterizerState* rasterizerStateCullNone;
};