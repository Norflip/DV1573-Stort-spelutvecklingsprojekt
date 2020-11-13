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
		LoadTextures(device);
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

	void LoadTextures(ID3D11Device* device) //VIKTOR
	{
		const LPCWSTR TEXTURE_PATHS[] = {
			L"Textures/Color_Ramp.png",
		
		};

		const size_t DIFFUSE_START_SLOT = 2;

		for (size_t i = 0; i < 1; i++)
		{
			rampTexture = Texture::LoadTexture(device, TEXTURE_PATHS[i]);
			material->SetTexture(rampTexture, DIFFUSE_START_SLOT + i, ShaderBindFlag::PIXEL);
		}

		material->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 2, ShaderBindFlag::PIXEL);
	}

	ALIGN16_ALLOC;

private:
	Material* material;
	ResourceManager* resources;
	Texture* rampTexture;
};
