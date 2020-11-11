#pragma once
#include "RenderPass.h"

ALIGN16
class FogRenderPass : public RenderPass
{
public:
	FogRenderPass(int priority, ResourceManager* resources) : RenderPass(priority, RenderPass::PassType::POST_PROCESSING), resources(resources)  {}

	void m_Initialize(ID3D11Device* device) override
	{
		Shader* shader = resources->GetShaderResource("fogShader");
		material = new Material(shader);
	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		renderer->ClearRenderTarget(outTexture, false);
		renderer->SetRenderTarget(outTexture, false);

		renderer->GetContext()->PSSetShaderResources(0, 1, &inTexture.srv);
		renderer->GetContext()->PSSetShaderResources(1, 1, &renderer->GetMidbuffer().depthSRV);
		renderer->DrawScreenQuad(material);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		renderer->GetContext()->PSSetShaderResources(1, 1, nullSRV);
	}

	ALIGN16_ALLOC;

private:
	Material* material;
	ResourceManager* resources;
};
