#pragma once
#include "RenderPass.h"

ALIGN16
class FogRenderPass : public RenderPass
{
public:
	FogRenderPass(int priority, ResourceManager* resources) : RenderPass(priority, RenderPass::PassType::POST_PROCESSING), resources(resources)  {}
	virtual ~FogRenderPass()
	{
		delete fogMaterial;
	}

	void m_Initialize(ID3D11Device* device) override
	{
		Shader* shader = resources->GetShaderResource("fogShader");
		fogMaterial = new Material(shader);
		LoadTexturesFog(device);
	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		renderer->ClearRenderTarget(outTexture, false);
		renderer->SetRenderTarget(outTexture, false);

		renderer->GetContext()->PSSetShaderResources(0, 1, &inTexture.srv);
		renderer->GetContext()->PSSetShaderResources(1, 1, &renderer->GetMidbuffer().depthSRV);
		renderer->DrawScreenQuad(fogMaterial);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		renderer->GetContext()->PSSetShaderResources(1, 1, nullSRV);
	}

	void LoadTexturesFog(ID3D11Device* device)
	{
		/*const LPCWSTR FOG_TEXTURE_PATHS[] = {
			L"Textures/Ramp_Day2.png",
			L"Textures/Ramp_Dusk.png",
			L"Textures/Ramp_Night.png",
			L"Textures/Ramp_End.png",
		};*/

		const std::string FOG_TEXTURE_PATHS[] = {
			"Day",
			"Dusk",
			"Night",
			"End",
		};

		const size_t DIFFUSE_START_SLOT = 2;

		for (size_t i = 0; i < 4; i++)
		{
			
			//rampTexture = Texture::LoadTexture(device, FOG_TEXTURE_PATHS[i]);
			rampTexture = resources->GetResource<Texture>(FOG_TEXTURE_PATHS[i]);
			fogMaterial->SetTexture(rampTexture, DIFFUSE_START_SLOT + i, ShaderBindFlag::PIXEL);
		}
		
		fogMaterial->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
	}

	ALIGN16_ALLOC;

private:
	Material* fogMaterial;
	ResourceManager* resources;
	Texture* rampTexture;
};
