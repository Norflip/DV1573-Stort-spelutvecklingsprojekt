#pragma once
//#include "stdafx.h"
#include "RenderPass.h"
#include "Shader.h"
//#include "ZWEBLoader.h"

ALIGN16
class GlowPreRenderPass : public RenderPass
{
public:

	GlowPreRenderPass(int priority, ResourceManager* resources)
		: RenderPass(priority, RenderPass::PassType::PRE_PASS) {}

	void m_Initialize(ID3D11Device* device) override
	{
		/*
			Sätt till en vettig width / height. 
		*/
		size_t TMP_WIDTH = 800;
		size_t TMP_HEIGHT = 800;
		ID3D11DepthStencilState* dss;

		glowTarget = DXHelper::CreateRenderTexture(TMP_WIDTH, TMP_HEIGHT, device, &dss);
	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) override
	{

		renderer->ClearRenderTarget(glowTarget, true);
		renderer->SetRenderTarget(glowTarget, true);

		// loop queue
		Renderer::RenderQueue renderq = renderer->GetEmissiveQueue();

		for (auto i : renderq)
		{
			while (!i.second.empty())
			{
				Renderer::RenderItem item = i.second.front();
				i.second.pop();

				// Om item är instancable.. kör en Instanceable_vs + emissive_ps t.ex
				// OM item är vanlig... kör en Default_vs + emissive_ps.


				// Denna drar ner prestandan.. men det visar att saker faktiskt finns i queuen.
				//std::cout << "GLOWING? : " << item.mesh->GetMeshName() << std::endl;
			}
		}

		// vi sparar glow texturer till renderer så vi sedan kan hämta den i nästa pass
		renderer->SetShaderResourceView("glow", target.srv);

		// vi sätter tillbaka till det vanliga render target för att renderar resten av scenen
		renderer->SetRenderTarget(current, true);
	}

	ALIGN16_ALLOC;

private:

	RenderTexture glowTarget;

};

ALIGN16
class GlowRenderPass : public RenderPass
{
public:

	GlowRenderPass(int priority, ResourceManager* resources)
		: RenderPass(priority, RenderPass::PassType::POST_PROCESSING), resources(resources), material(nullptr) {}

	void m_Initialize(ID3D11Device* device) override
	{
		// denna är bara renderpass shadern.
		Shader* shader = resources->GetShaderResource("GlowShader");
		material = new Material(shader);
	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) override
	{
		renderer->ClearRenderTarget(target, false);
		renderer->SetRenderTarget(target, false);

		ID3D11ShaderResourceView* srv = renderer->GetShaderResourceView("glow");

		// I renderpass shadern GlowShader så blir första texturen scenen i sig och den andra all data från glow texturen som vi gjorde i tidigare GlowPreRenderPass

		renderer->GetContext()->PSSetShaderResources(0, 1, &current.srv);
		renderer->GetContext()->PSSetShaderResources(1, 1, &srv);

		renderer->DrawScreenQuad(material);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		renderer->GetContext()->PSSetShaderResources(0, 1, nullSRV);
		renderer->GetContext()->PSSetShaderResources(1, 1, nullSRV);
	}

	ALIGN16_ALLOC;

private:
	Shader* shader;
	Material* material;
	ResourceManager* resources;

};