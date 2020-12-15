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
		shader->SetVertexShader("Shaders/Default_vs.hlsl");
		shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
		shader->Compile(device);
		shaderInstanced->SetVertexShader("Shaders/Instance_vs.hlsl");
		//shaderInstanced->SetInputLayoutStructure(9, shaderInstanced->INSTANCE_INPUT_LAYOUTd);
		shaderInstanced->SetPixelShader("Shaders/Emissive_ps.hlsl");
		shaderInstanced->Compile(device);

		material = new Material(shader);
		materialInstanced = new Material(shaderInstanced);
	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) override
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		renderer->GetContext()->PSSetShaderResources(2, 1, &nullSRV);
		renderer->ClearRenderTarget(glowTarget, true);
		renderer->SetRenderTarget(glowTarget, true);
		
		// loop queue
		Renderer::RenderQueue renderq = renderer->GetEmissiveQueue();

		for (auto i : renderq)
		{
			while (!i.second.empty())
			{
				Renderer::RenderItem item = i.second.front();

				//i.second.pop();

				// Om item är instancable.. kör en Instanceable_vs + emissive_ps t.ex
				// OM item är vanlig... kör en Default_vs + emissive_ps.

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
				/*if (item.type == Renderer::RenderItem::Type::Instanced)
				{
					//shader->SetVertexShader("Shaders/Instance_vs.hlsl");
					//shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
					//shader->SetPixelShader("Shaders/Default_ps.hlsl");
					std::cout << "Get Into Instance case" << std::endl;
					item.material = materialInstanced;

				}
				else if (item.type == Renderer::RenderItem::Type::Default)
				{
					//shader->SetVertexShader("Shaders/Default_vs.hlsl");
					//shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
					std::cout << "Get Into default case" << std::endl;
					item.material = material;
				}
				else
				{
					std::cout << "got nowhere" << std::endl;
				}*/
				switch (item.type)
				{
					case Renderer::RenderItem::Type::Instanced:
						//std::cout << "Get Into Instance case" << std::endl;
						renderer->DrawRenderItemInstanced(item, camera);
						break;

					case Renderer::RenderItem::Type::Default:
					default:
						//std::cout << "Get Into default case" << std::endl;
						renderer->DrawRenderItem(item, camera);
						break;
				}

				renderer->GetContext()->PSSetShaderResources(0, 1, &target.srv);
				// Denna drar ner prestandan.. men det visar att saker faktiskt finns i queuen.
				//std::cout << "GLOWING? : " << item.mesh->GetMeshName()<< " " << i.second.size() << std::endl;
				i.second.pop();

			}
		}

		// UNBINDA GLOWTARGET HÄR. Kommer ge fel annars
		//material->UnbindToContext(renderer->GetContext());
		//materialInstanced->UnbindToContext(renderer->GetContext());


		// vi sparar glow texturer till renderer så vi sedan kan hämta den i nästa pass
		renderer->StoreShaderResourceView("glow", target.srv);
		renderer->GetContext()->PSSetShaderResources(0, 1, nullptr);

		// vi sätter tillbaka till det vanliga render target för att renderar resten av scenen
		renderer->SetRenderTarget(current, true);
	}

	ALIGN16_ALLOC;

private:

	RenderTexture glowTarget;
	Shader* shader;
	Shader* shaderInstanced;
	Material* material;
	Material* materialInstanced;
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
		/*renderer->ClearRenderTarget(current, false);
		renderer->SetRenderTarget(current, false);*/
		

		//ID3D11ShaderResourceView* srv = renderer->LoadShaderResourceView("glow");

		// I renderpass shadern GlowShader så blir första texturen scenen i sig och den andra all data från glow texturen som vi gjorde i tidigare GlowPreRenderPass

		renderer->GetContext()->PSSetShaderResources(0, 1, &current.srv);
		renderer->GetContext()->PSSetShaderResources(2, 1, &current.srv);

		renderer->DrawScreenQuad(material);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		//renderer->GetContext()->PSSetShaderResources(0, 1, nullSRV);
		renderer->GetContext()->PSSetShaderResources(2, 1, nullSRV);
	}

	ALIGN16_ALLOC;

private:
	Shader* shader;
	Material* material;
	ResourceManager* resources;

};