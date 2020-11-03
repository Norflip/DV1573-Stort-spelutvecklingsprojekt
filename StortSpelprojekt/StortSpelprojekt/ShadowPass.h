#pragma once
#include "RenderPass.h"
#include "DXHelper.h"

constexpr const char* SHADOW_SAVE_NAME = "shadowDepth";
constexpr size_t SHADOW_TARGET_WIDTH = 256;
constexpr size_t SHADOW_TARGET_HEIGHT = 256;

ALIGN16
class ShadowDepthPrePass : public RenderPass
{
public:
	ShadowDepthPrePass(int priority) : RenderPass(priority, RenderPass::PassType::PRE_PASS), lights(LightManager::Instance()), depthRenderMaterial(nullptr)
	{
	}

	void m_Initialize(ID3D11Device* device) override
	{
		shadowTarget = DXHelper::CreateRenderTexture(2048, 2048, device);

		Shader* shader0 = new Shader;
		shader0->SetPixelShader("Shaders/DepthRender_ps.hlsl", "main");
		shader0->SetVertexShader("Shaders/DepthRender_vs.hlsl");
		shader0->Compile(device);

		depthRenderMaterial = new Material(shader0);
	}

	void Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		VirtualCamera* shadowcamera = lights.GetShadowCamera();
		dx::XMMATRIX shadowCameraTransform = lights.GetShadowCameraTransform();

		// LOOPA QUEUES
		renderer->ClearRenderTarget(shadowTarget, true);
		renderer->SetRenderTarget(shadowTarget, true);
		renderer->SetCullMode(D3D11_CULL_FRONT);

		renderer->DrawQueueForPass(shadowcamera, shadowCameraTransform, renderer->GetOpaqueItemQueue());
		//renderer->DrawQueueForPass(shadowcamera, shadowCameraTransform, renderer->GetTransparentItemQueue());

		renderer->SaveShaderResourceView(SHADOW_SAVE_NAME, shadowTarget.depthSRV);
	}

	ALIGN16_ALLOC;

private:
	Material* depthRenderMaterial;
	RenderTexture shadowTarget;
	LightManager& lights;
};


ALIGN16
class ShadowRenderPass : public RenderPass
{
public:
	ShadowRenderPass(int priority) : RenderPass(priority, RenderPass::PassType::POST_PASS), shadowPassMaterial(nullptr) {}

	void m_Initialize(ID3D11Device* device) override
	{		
		Shader* shader1 = new Shader;
		shader1->SetPixelShader("Shaders/ShadowPass_ps.hlsl", "main");
		shader1->SetVertexShader("Shaders/ScreenQuad_vs.hlsl");
		shader1->Compile(device);

		shadowPassMaterial = new Material(shader1);	
	}

	void Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture) override
	{
		// SÄTTA SHADOW KAMERA TILL RENDERTARGET
		
		renderer->ClearRenderTarget(outTexture, false);
		renderer->SetRenderTarget(outTexture, false);

		ID3D11DeviceContext* context = renderer->GetContext();
		ID3D11ShaderResourceView* shadowDepth = renderer->LoadShaderResourceView(SHADOW_SAVE_NAME);

		context->PSSetShaderResources(0, 1, &inTexture.srv);
		context->PSSetShaderResources(1, 1, &renderer->GetMidbuffer().depthSRV);
		context->PSSetShaderResources(2, 1, &shadowDepth);

		renderer->DrawScreenQuad(shadowPassMaterial);
		shadowPassMaterial->UnbindToContext(context);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		renderer->GetContext()->PSSetShaderResources(0, 1, nullSRV);
		renderer->GetContext()->PSSetShaderResources(1, 1, nullSRV);
		renderer->GetContext()->PSSetShaderResources(2, 1, nullSRV);
	}

	ALIGN16_ALLOC;

private:
	Material* shadowPassMaterial;

};
