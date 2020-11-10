#pragma once
#include "RenderPass.h"
#include "Shader.h"
#include "ZWEBLoader.h"
namespace dx = DirectX;

ALIGN16
class SkyboxRenderPass : public RenderPass
{
public:
	SkyboxRenderPass(int priority, ResourceManager* resources);
	virtual ~SkyboxRenderPass();

	void m_Initialize(ID3D11Device* device) override;

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture) override;

	void LoadTextures(ID3D11Device* device);

	ALIGN16_ALLOC;
	Material* GetMaterial() const { return this->material; }

private:
	ResourceManager* resources;
	Material* material;
	Mesh* mesh;
	Texture* textures [4];

	dx::XMMATRIX scaleRot;
};
