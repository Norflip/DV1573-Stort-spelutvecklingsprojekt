#pragma once

#include "DXHelper.h"
#include "Mesh.h"
#include "Buffers.h"
#include "Texture.h"
#include "Material.h"
#include "CameraComponent.h"
#include "LightManager.h"
#include "ConstantBuffer.h"
#include "DepthPass.h"
#include <time.h>

#include "Particlesys.h"

inline int GetBatchID(const Material* material, const Mesh* mesh)
{
	return std::hash<int>()(material->GetID() + 100) ^ std::hash<int>()(mesh->GetID());
}

class RenderPass;

ALIGN16
class Renderer
{
	//const FLOAT DEFAULT_BG_COLOR[4] = { 0.3f, 0.1f, 0.2f, 1.0f };
	const FLOAT DEFAULT_BG_COLOR[4] = { 0.2f, 0.2f, 0.2f, 1.0f };

	static const int MAX_BATCH_COUNT = 512;
	struct Batch
	{
		const Material* material;
		const Mesh* mesh;
		std::vector<dx::XMFLOAT4X4> transformations;
	};

public:
	struct RenderItem
	{
		enum class Type
		{
			Default,
			Instanced,
			Skeleton,
			Grass,
			Particles,
			NewParticles
		};

		const Mesh* mesh;
		const Material* material;
		const Material* streamoutMaterial;	// New
		
		cb_particle* particles; // new

		Type type;		

		std::vector<dx::XMFLOAT4X4>* bones;
		dx::XMMATRIX world;
		
		ID3D11Buffer* instanceBuffer;
		size_t instanceCount;
	};

	typedef std::unordered_map<size_t, std::queue<RenderItem>> RenderQueue;

public:
	Renderer();
	virtual ~Renderer();

	void Initialize(Window* window);

	void DrawQueueToTarget(RenderQueue& queue, CameraComponent* camera, bool bindMaterial = true, bool pop = true);
	void RenderFrame(CameraComponent* camera, float time, float distance = 1.f, bool useImgui = true);
	void RenderFrame(CameraComponent* camera, float time, float distance, RenderTexture& target, bool drawGUI = false, bool applyRenderPasses = true);

	void AddRenderPass(RenderPass*);
	
	void Draw(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model, bool batchable);
	void DrawInstanced(const Mesh* mesh, const size_t& count, ID3D11Buffer* instanceBuffer, const Material* material);
	void DrawSkeleton(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model, std::vector<dx::XMFLOAT4X4>& bones);
	void DrawGrass(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model);
	void DrawParticles(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model);
	void DrawNewParticles(const Mesh* particleMesh, const Material* drawMat, const Material* streamoutMat, cb_particle* particleData);
	void DrawImmediate(const Mesh* mesh, const Material* material, const CameraComponent* camera, const dx::XMMATRIX& model);
	//void DrawEmissive(const Mesh* mesh, const Material* material, const dx::XMMATRIX& model);
	
	void SetCullBack(bool);

	ID3D11Device* GetDevice() const { return this->device; }
	ID3D11DeviceContext* GetContext() const { return this->context; }
	Window* GetOutputWindow() const { return this->window; }

	void DrawScreenQuad(const Material* Material);

	void ClearRenderTarget(const RenderTexture& target, bool clearDepth = true);
	void SetRenderTarget(const RenderTexture& target, bool setDepth = true);

	bool IsDrawingShapes() const { return this->drawShapes; }
	void DrawShapes(bool draw) { this->drawShapes = draw; }

	RenderTexture& GetMidbuffer() { return this->midbuffer; }

	void RemoveRenderPass(RenderPass*);

	void InitForwardPlus(VirtualCamera* camera);
	void UpdateForwardPlus(CameraComponent* camera);
	//std::vector<UINT>& cullLightsOnCPU();
	void SetIdAndColor(int id, float color);

	void ListParticle(Particlesys* psys) { this->particleList.push_back(psys); }
	std::vector<Particlesys*> GetParticleList() { return this->particleList; }

	/* new stuffy */
	void EnableAlphaBlending();
	void DisableAlphaBlending();	
	ID3D11DepthStencilState* GetDepthEnable() { return this->dss_On; }
	ID3D11DepthStencilState* GetDepthDisable() { return this->dss_Off; }

	ALIGN16_ALLOC;

	void StoreShaderResourceView(const std::string& key, ID3D11ShaderResourceView* srv);
	ID3D11ShaderResourceView* LoadShaderResourceView(const std::string& key) const;

	RenderQueue& GetEmissiveQueue() { return this->emissiveItemQueue; }
	const RenderQueue& GetEmissiveQueue() const  { return this->emissiveItemQueue; }

private:

	void AddItem(const RenderItem& item, bool transparent, bool cullDepth, bool emissive = false);
	void DrawRenderItem(const RenderItem& item, CameraComponent* camera);
	void DrawRenderItemInstanced(const RenderItem& item, CameraComponent* camera);
	void DrawRenderItemSkeleton(const RenderItem& item, CameraComponent* camera);
	void DrawRenderItemGrass(const RenderItem& item, CameraComponent* camera);
	void DrawRenderItemParticles(const RenderItem& item, CameraComponent* camera);
	void DrawRenderItemNewParticles(const RenderItem& item, CameraComponent* camera);
	void DrawBatch(const Batch& batch, CameraComponent* camera);

	void SetObjectBufferValues(const CameraComponent* camera, dx::XMMATRIX world, bool transpose);
	Mesh* CreateScreenQuad();


private:
	IDXGISwapChain* swapchain;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
		
	RenderTexture backbuffer;
	RenderTexture midbuffer;
	RenderTexture renderPassSwapBuffers [2];
	
	Material* screenQuadMaterial;
	Mesh* screenQuadMesh;
	Shader forwardPlusShader;
	bool forwardPlusInitialized;

	ConstantBuffer<cb_particle> particleBuffer;
	ConstantBuffer<cb_Object> objectBuffer;
	ConstantBuffer<cb_Scene> sceneBuffer;
	ConstantBuffer<cb_Material> materialBuffer;
	ConstantBuffer<cb_DispatchParams> dispatchParamsBuffer; //F+
	ConstantBuffer<cb_ScreenToViewParams> screenToViewParams;//F+

	UINT width; //F+
	UINT height;//F+

	dx::XMUINT3 numThreadGroups;
	dx::XMUINT3 numThreads;
	//Frustums
	
	std::vector<s_Frustum> frustum_data;
	ID3D11Buffer* frustums_buffer = 0;
	ID3D11ShaderResourceView* inFrustums_srv = 0;
	ID3D11UnorderedAccessView* outFrustums_uav = 0;

	//light index counter
	std::vector<UINT> o_LightIndexCounter;
	ID3D11Buffer* o_LightIndexCounter_uavbuffer = 0;
	ID3D11UnorderedAccessView* o_LightIndexCounter_uav = 0;
	std::vector<UINT> t_LightIndexCounter;
	ID3D11Buffer* t_LightIndexCounter_uavbuffer = 0;
	ID3D11UnorderedAccessView* t_LightIndexCounter_uav = 0;
	
	//light index list
	std::vector<UINT> o_LightIndexList;
	ID3D11Buffer* o_LightIndexList_uavbuffer = 0;
	ID3D11ShaderResourceView* o_LightIndexList_srv = 0;
	ID3D11UnorderedAccessView* o_LightIndexList_uav = 0;
	std::vector<UINT> t_LightIndexList;
	ID3D11Buffer* t_LightIndexList_uavbuffer = 0;
	ID3D11ShaderResourceView* t_LightIndexList_srv = 0;
	ID3D11UnorderedAccessView* t_LightIndexList_uav = 0;

	//light grid
	
	ID3D11UnorderedAccessView* o_LightGrid_tex = 0;
	ID3D11ShaderResourceView* o_LightGrid_texSRV = 0;
	ID3D11UnorderedAccessView* t_LightGrid_tex = 0;
	ID3D11ShaderResourceView* t_LightGrid_texSRV = 0;


	std::vector<dx::XMFLOAT4X4> srv_skeleton_data;
	ID3D11Buffer* skeleton_srvbuffer;
	ID3D11ShaderResourceView* skeleton_srv;

	Window* window;

	std::unordered_map<int, Batch> opaqueBatches;
	std::unordered_map<int, Batch> transparentBatches;
	std::unordered_map<int, Batch> emissiveBatches;
	ID3D11Buffer* batchInstanceBuffer;
	dx::XMFLOAT4X4* tmpBatchInstanceData;

	RenderQueue opaqueItemQueue;
	RenderQueue transparentItemQueue;
	RenderQueue emissiveItemQueue;

	std::vector<RenderPass*> passes;
	std::unordered_map<std::string, ID3D11ShaderResourceView*> storedSRVs;
	//blendstate
	ID3D11BlendState* blendStateOn;
	ID3D11BlendState* blendStateOff;
	
	ID3D11DepthStencilState* dss;	

	DepthPass depthPass;
	const float BLENDSTATEMASK[4] = { 0.0f };
	
	bool drawShapes = true;

	bool isFullScreen = false;

	float xPos= 0;
	float yPos =0;

	int ids;
	float color;

	//rasterizer
	ID3D11RasterizerState* rasterizerStateCullBack;
	ID3D11RasterizerState* rasterizerStateCullNone;
	ID3D11RasterizerState* rasterizerStateCCWO;


	/* new stuff */
	bool firstRun;
	std::vector<Particlesys*> particleList;
	ID3D11BlendState* particleBlendOn;
	ID3D11BlendState* particleBlendOff;
	ID3D11DepthStencilState* dss_Off;
	ID3D11DepthStencilState* dss_On;
};