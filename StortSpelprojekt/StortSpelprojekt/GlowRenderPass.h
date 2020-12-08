#pragma once
//#include "stdafx.h"
#include "RenderPass.h"
#include "Shader.h"
//#include "ZWEBLoader.h"

ALIGN16
class GlowRenderPass : public RenderPass
{
public:

	GlowRenderPass(int priority, ResourceManager* resources)
		: RenderPass(priority, RenderPass::PassType::POST_PROCESSING), resources(resources), glowMaterial(nullptr), mesh(nullptr) {}

	void m_Initialize(ID3D11Device* device) override
	{
		Shader* shader = resources->GetShaderResource("GlowShader");
		//shader->SetVertexShader("Shaders/Glow_vs.hlsl");
		//shader->SetPixelShader("Shaders/Glow_ps.hlsl");
		//shader->Compile(device);
		glowMaterial = new Material(shader);
		glowMaterial->SetEmissive(true);
		//LoadTextureGlow(device);
	}
	
	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) override
	{
		renderer->ClearRenderTarget(target, false);
		renderer->SetRenderTarget(target, false);

		renderer->GetContext()->PSSetShaderResources(0, 1, &current.srv);
		
		renderer->DrawScreenQuad(glowMaterial);
		//ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		//renderer->GetContext()->PSSetShaderResources(1, 1, nullSRV);
	}

	/*void LoadTextureGlow(ID3D11Device* device)
	{
		const LPCWSTR GLOW_TEXTURE_PATH = L"Textures/lampa_glowMap.png";

		glowTexture = Texture::LoadTexture(device, GLOW_TEXTURE_PATH);
		glowMaterial->SetTexture(glowTexture, 0, ShaderBindFlag::PIXEL);

		glowMaterial->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
	}*/

	ALIGN16_ALLOC;

private:

	Mesh* mesh;
	Material* glowMaterial;
	ResourceManager* resources;
	Texture* glowTexture;

};