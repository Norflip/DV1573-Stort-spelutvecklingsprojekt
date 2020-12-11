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
			S�tt till en vettig width / height. 
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
			//while (i.second.size() != 0)
			{
				Renderer::RenderItem item = i.second.front();

				// Om item �r instancable.. k�r en Instanceable_vs + emissive_ps t.ex
				// OM item �r vanlig... k�r en Default_vs + emissive_ps.

				//switch (item.type)
				//{
				//	case Renderer::RenderItem::Type::Instanced:
				//		/*shader->SetVertexShader("Shaders/Instance_vs.hlsl");
				//		shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
				//		material = new Material(shader);
				//		renderer->GetContext()->PSSetShaderResources(0, 1, &target.srv);
				//		renderer->DrawScreenQuad(material);*/
				//		//shader->SetVertexShader("Shaders/Instance_vs.hlsl");
				//		//shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
				//		//renderer->LoadShaderResourceView("glow");
				//		std::cout << "Get Into Instance case" << std::endl;
				//		
				//		break;
				//
				//	case Renderer::RenderItem::Type::Default:
				//	default:
				//		/*shader->SetVertexShader("Shaders/Default_vs.hlsl");
				//		shader->SetPixelShader("Shaders/Default_ps.hlsl");
				//		material = new Material(shader);
				//		renderer->GetContext()->PSSetShaderResources(0, 1, &target.srv);
				//		renderer->DrawScreenQuad(material);*/
				//		//shader->SetVertexShader("Shaders/Default_vs.hlsl");
				//		//shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
				//		//renderer->LoadShaderResourceView("default");
				//		
				//		break;
				//
				//}
				/*item.material = new Material(shader);
				renderer->GetContext()->PSSetShaderResources(0, 1, &target.srv);
				renderer->DrawScreenQuad(item.material);*/



				if (item.type == Renderer::RenderItem::Type::Instanced)
				{
					//shader->SetVertexShader("Shaders/Instance_vs.hlsl");
					//shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
					//shader->SetPixelShader("Shaders/Default_ps.hlsl");
					std::cout << "Get Into Instance case" << std::endl;

				}
				else if (item.type == Renderer::RenderItem::Type::Default)
				{
					shader->SetVertexShader("Shaders/Default_vs.hlsl");
					shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
					std::cout << "Get Into default case" << std::endl;
				}
				else
				{
					std::cout << "got nowhere" << std::endl;

				}

				item.material = new Material(shader);
				renderer->GetContext()->PSSetShaderResources(0, 1, &target.srv);
				renderer->DrawScreenQuad(item.material);

				// Denna drar ner prestandan.. men det visar att saker faktiskt finns i queuen.
				//std::cout << "GLOWING? : " << item.mesh->GetMeshName()<< " " << i.second.size() << std::endl;

				i.second.pop();

			}
		}

		// UNBINDA GLOWTARGET H�R. Kommer ge fel annars
		ID3D11RenderTargetView* null = nullptr;
		renderer->GetContext()->OMSetRenderTargets(1, &glowTarget.rtv, glowTarget.dsv);



		// vi sparar glow texturer till renderer s� vi sedan kan h�mta den i n�sta pass
		renderer->StoreShaderResourceView("glow", target.srv);
		//renderer->GetContext()->PSSetShaderResources(0, 1, nullptr);

		// vi s�tter tillbaka till det vanliga render target f�r att renderar resten av scenen
		renderer->SetRenderTarget(current, true);
	}

	ALIGN16_ALLOC;

private:

	RenderTexture glowTarget;
	Shader* shader = new Shader;
	Material* material;
};

ALIGN16
class GlowRenderPass : public RenderPass
{
public:

	GlowRenderPass(int priority, ResourceManager* resources)
		: RenderPass(priority, RenderPass::PassType::POST_PROCESSING), resources(resources), material(nullptr) {}

	void m_Initialize(ID3D11Device* device) override
	{
		// denna �r bara renderpass shadern.
		Shader* shader = resources->GetShaderResource("GlowShader");
		material = new Material(shader);
	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) override
	{
		renderer->ClearRenderTarget(target, false);
		renderer->SetRenderTarget(target, false);

		ID3D11ShaderResourceView* srv = renderer->LoadShaderResourceView("glow");

		// I renderpass shadern GlowShader s� blir f�rsta texturen scenen i sig och den andra all data fr�n glow texturen som vi gjorde i tidigare GlowPreRenderPass

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