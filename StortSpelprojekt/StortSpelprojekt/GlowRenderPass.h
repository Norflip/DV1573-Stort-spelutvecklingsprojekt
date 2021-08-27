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
		/*size_t TMP_WIDTH = 800;
		size_t TMP_HEIGHT = 800;*/

		size_t TMP_WIDTH = 960; // 1920 / 2
		size_t TMP_HEIGHT = 540; // 1080 / 2

		
		// HÄMTA FRÅN WINDOW

		ID3D11DepthStencilState* dss;

		glowTarget = DXHelper::CreateRenderTexture(TMP_WIDTH, TMP_HEIGHT, device, &dss);

		shader = new Shader();
		shader->SetVertexShader("Shaders/Default_vs.hlsl");
		shader->SetPixelShader("Shaders/Emissive_ps.hlsl");
		//shader->SetComputeShader("Shaders/FirstPassBloom.hlsl");
		shader->Compile(device);

		shaderInstanced = new Shader();
		shaderInstanced->SetVertexShader("Shaders/Instance_vs.hlsl");
		shaderInstanced->SetInputLayoutStructure(9, shaderInstanced->INSTANCE_INPUT_LAYOUTd);
		shaderInstanced->SetPixelShader("Shaders/Emissive_ps.hlsl");
		//shaderInstanced->SetComputeShader("Shaders/FirstPassBloom.hlsl");
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
		

		// SÄTT DEPTH TILL DEN SOM RITADE UT OPAQUE
		// gör så att saker som inte ska synas inte syns



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

				switch (item.type)
				{
					case Renderer::RenderItem::Type::Instanced:
						//std::cout << "Get Into Instance case" << std::endl;
						materialInstanced->SetTexture(item.material->GetTexture(2, ShaderBindFlag::PIXEL), 2, ShaderBindFlag::PIXEL);
						materialInstanced->BindToContext(renderer->GetContext());
						renderer->DrawRenderItemInstanced(item, camera);
						break;

					case Renderer::RenderItem::Type::Default:
					default:
						//std::cout << "Get Into default case" << std::endl;
						//item.material->GetTexture(2, ShaderBindFlag::PIXEL);
						material->SetTexture(item.material->GetTexture(2, ShaderBindFlag::PIXEL), 2, ShaderBindFlag::PIXEL);
						material->BindToContext(renderer->GetContext());
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
		material->UnbindToContext(renderer->GetContext());
		materialInstanced->UnbindToContext(renderer->GetContext());

		// vi sparar glow texturer till renderer så vi sedan kan hämta den i nästa pass
		renderer->StoreShaderResourceView("glow", glowTarget.srv);
		//renderer->GetContext()->PSSetShaderResources(0, 1, nullptr);

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

		csshader = new Shader();
		csshader->SetComputeShader("Shaders/FirstPassBloom.hlsl");
		csshader->Compile(device);

		size_t TMP_WIDTH = 960; // 1920 / 2
		size_t TMP_HEIGHT = 540; // 1080 / 2

		D3D11_TEXTURE2D_DESC tdesc;
		ZeroMemory(&tdesc, sizeof(tdesc));
		tdesc.Width = TMP_WIDTH;
		tdesc.Height = TMP_HEIGHT;
		tdesc.MipLevels = 1;
		tdesc.ArraySize = 1;
		tdesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		tdesc.SampleDesc.Count = 1;
		tdesc.SampleDesc.Quality = 0;
		tdesc.Usage = D3D11_USAGE_DEFAULT;
		tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		tdesc.CPUAccessFlags = 0;
		tdesc.MiscFlags = 0;

		HRESULT hr;
		hr = device->CreateTexture2D(&tdesc, 0, &tex);
		assert(SUCCEEDED(hr));


		// Unordered access view
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavdesc;
		ZeroMemory(&uavdesc, sizeof(uavdesc));
		uavdesc.Format = DXGI_FORMAT_UNKNOWN;
		uavdesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavdesc.Texture2D.MipSlice = 0;
		hr = device->CreateUnorderedAccessView(tex, &uavdesc, &uav);
		assert(SUCCEEDED(hr));

		// shader resource view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
		srvdesc.Format = tdesc.Format;
		srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvdesc.Texture2D.MostDetailedMip = 0;
		srvdesc.Texture2D.MipLevels = 1;
		hr = device->CreateShaderResourceView(tex, &srvdesc, &srv);
		assert(SUCCEEDED(hr));

	}

	void Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& current, RenderTexture& target) override
	{
		renderer->ClearRenderTarget(target, false);
		renderer->SetRenderTarget(target, false);

		ID3D11ShaderResourceView* emissive_srv = renderer->LoadShaderResourceView("glow");

	
		// BLURRA HÄR
		size_t TMP_WIDTH = 960; // 1920 / 2
		size_t TMP_HEIGHT = 540; // 1080 / 2

		ID3D11DeviceContext* ctx = renderer->GetContext();



		csshader->BindToContext(ctx);

		ctx->CSSetShaderResources(0, 1, &emissive_srv);
		ctx->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);

		const int numthreads_x = TMP_WIDTH / 8;
		const int numthreads_y = TMP_HEIGHT / 8;
		ctx->Dispatch(numthreads_x, numthreads_y, 1);

		// kom ihåg att SRV och UAV kan inte vara bindade samtidigt
		

		// UNBIND SHIET
		csshader->Unbind(ctx);

		ID3D11ShaderResourceView* nullsrv[1] = { nullptr };
		ctx->CSSetShaderResources(0, 1, nullsrv);

		ID3D11UnorderedAccessView* nulluav[1] = { nullptr };
		ctx->CSSetUnorderedAccessViews(0, 1, nulluav, nullptr);



		// I renderpass shadern GlowShader så blir första texturen scenen i sig och den andra all data från glow texturen som vi gjorde i tidigare GlowPreRenderPass
		
		
		// BINDA DEN NYA TEXTUREN ISTÄLLET FÖR GLOW_SRV
		ctx->PSSetShaderResources(0, 1, &current.srv);
		ctx->PSSetShaderResources(2, 1, &srv);
		ctx->PSSetShaderResources(3, 1, &emissive_srv);

		//renderer->GetContext()->PSSetShaderResources(2, 1, &srv);

		renderer->DrawScreenQuad(material);

		ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
		//renderer->GetContext()->PSSetShaderResources(0, 1, nullSRV);
		ctx->PSSetShaderResources(2, 1, nullSRV);
	}

	void executeComputeShader(ID3D11DeviceContext* context, ID3D11ComputeShader* shader, UINT uinputNum,
		ID3D11UnorderedAccessView** UAVInputsPtrPtr, UINT x, UINT y, UINT z, RenderTexture& srv, Renderer renderer, ID3D11Buffer* bufferptr)
	{
		context->CSSetShader(shader, nullptr, 0);

		context->CSSetShaderResources(0, 1, &srv.srv);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32_UINT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		

		context->CSGetUnorderedAccessViews(0, uinputNum, UAVInputsPtrPtr);

		//context->UpdateSubresource(bufferptr, 0, nullptr, , 0, 0);
		//context->

		//DXHelper::CreateStructuredBuffer(renderer.GetDevice(),)

		/*
		
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_R32G32_UINT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32G32_UINT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;

		device->CreateUnorderedAccessView(tex2D, &uavDesc, &o_LightGrid_tex);
		device->CreateUnorderedAccessView(tex2D2, &uavDesc, &t_LightGrid_tex);
		device->CreateShaderResourceView(tex2D, &srvDesc, &o_LightGrid_texSRV);
		device->CreateShaderResourceView(tex2D, &srvDesc, &t_LightGrid_texSRV);
		tex2D->Release();
		tex2D2->Release();
		
		*/

	}

	ALIGN16_ALLOC;

private:
	Shader* shader;
	Shader* csshader;
	Material* material;
	ResourceManager* resources;

	ID3D11Texture2D* tex;
	ID3D11UnorderedAccessView* uav;
	ID3D11ShaderResourceView* srv;

};