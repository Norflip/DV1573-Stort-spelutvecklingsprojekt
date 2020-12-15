#pragma once
#include "RenderPass.h"

ALIGN16
class FXAARenderPass : public RenderPass
{
public:
	FXAARenderPass(int priority, ResourceManager* resources) : RenderPass(priority, RenderPass::PassType::POST_PROCESSING), resources(resources) {}

	virtual ~FXAARenderPass()
	{
		delete material;
	}

	void m_Initialize(ID3D11Device* device) override
	{
		Shader* shader = resources->GetShaderResource("FXAAShader");
		material = new Material(shader);

		//auto sampler = DXHelper::CreateSampler(D3D11_FILTER_MAXIMUM_ANISOTROPIC , D3D11_TEXTURE_ADDRESS_WRAP, device);
		auto sampler = DXHelper::CreateSampler(D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP, device);
		material->SetSampler(sampler, 0, ShaderBindFlag::PIXEL);
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