#pragma once
#include "RenderPass.h"

ALIGN16
class GlowRenderPass : public RenderPass
{
public:

	GlowRenderPass(int priority, ResourceManager* resources) : RenderPass(priority, RenderPass::PassType::POST_PROCESSING), resources(resources) {}

	void m_Initialize(ID3D11Device* device) override
	{
		Shader* shader = resources->GetShaderResource("GlowShader");
		glowMaterial = new Material(shader);
		shader->SetVertexShader("Shaders/Glow_vs.hlsl");
		LoadTextureGlow(device);
		shader->Compile(device);
	}
	
	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) override
	{
		renderer->ClearRenderTarget(target, false);
		renderer->SetRenderTarget(target, false);

		renderer->GetContext()->PSSetShaderResources(0, 1, &current.srv);
		renderer->GetContext()->PSSetShaderResources(0, 1, &renderer->GetMidbuffer().depthSRV);
		renderer->DrawScreenQuad(glowMaterial);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		renderer->GetContext()->PSSetShaderResources(0, 1, nullSRV);
	}

	void LoadTextureGlow(ID3D11Device* device)
	{
		const LPCWSTR GLOW_TEXTURE_PATH = L"Textures/lampa_glowMap.png";

		glowTexture = Texture::LoadTexture(device, GLOW_TEXTURE_PATH);
		glowMaterial->SetTexture(glowTexture, 1, ShaderBindFlag::PIXEL);

		glowMaterial->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
	}

	ALIGN16_ALLOC;

private:
	Material* glowMaterial;
	ResourceManager* resources;
	Texture* glowTexture;

};