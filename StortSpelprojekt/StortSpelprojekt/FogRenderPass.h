#pragma once
#include "RenderPass.h"

class FogRenderPass : public RenderPass
{
public:
	FogRenderPass(int priority) : RenderPass(priority)  {}

	void m_Initialize(ID3D11Device* device) override
	{
		Shader* shader = new Shader;
		shader->SetPixelShader("Shaders/FogTest_ps.hlsl", "main");
		shader->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
		shader->Compile(device);
		material = Material(shader);
	}

	bool Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		renderer->GetContext()->PSSetShaderResources(1, 1, &renderer->GetMidbuffer().depthSRV);

		renderer->DrawScreenQuad(material);
		return true;
	}

private:
	Material material;

};
