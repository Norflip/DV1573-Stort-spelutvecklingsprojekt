#pragma once
#include "RenderPass.h"

class FogRenderPass : public RenderPass
{
public:
	FogRenderPass(int priority) : RenderPass(priority, RenderPass::PassType::POST_PROCESSING)  {}

	void m_Initialize(ID3D11Device* device) override
	{
		Shader* shader = new Shader;
		shader->SetPixelShader("Shaders/FogTest_ps.hlsl", "main");
		shader->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
		shader->Compile(device);
		material = Material(shader);
	}

	void Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		renderer->ClearRenderTarget(outTexture, false);
		renderer->SetRenderTarget(outTexture, false);

		renderer->GetContext()->PSSetShaderResources(0, 1, &inTexture.srv);
		renderer->GetContext()->PSSetShaderResources(1, 1, &renderer->GetMidbuffer().depthSRV);
		renderer->DrawScreenQuad(material);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		renderer->GetContext()->PSSetShaderResources(1, 1, nullSRV);
	}

private:
	Material material;

};