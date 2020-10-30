#include "stdafx.h"
#include "GUIManager.h"





GUIManager::GUIManager(Renderer* renderer, int priority) : RenderPass(priority, RenderPass::PassType::UI_OVERLAY), renderer(renderer)
{
	spriteBatch = new DirectX::SpriteBatch(this->renderer->GetContext());


	//Desc for blending png files with depth//

	D3D11_DEPTH_STENCIL_DESC dsDesc;
	dsDesc.DepthEnable = FALSE;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
	dsDesc.StencilEnable = false;
	dsDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	dsDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsDesc.BackFace = dsDesc.FrontFace;
	renderer->GetDevice()->CreateDepthStencilState(&dsDesc, &depthStencilState);
	HRESULT hr;
	D3D11_BLEND_DESC blendDescGui;
	ZeroMemory(&blendDescGui, sizeof(D3D11_BLEND_DESC));
	//blendDescGui.AlphaToCoverageEnable = true;
	//blendDescGui.IndependentBlendEnable = true;
	blendDescGui.RenderTarget[0].BlendEnable = TRUE;
	blendDescGui.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescGui.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescGui.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescGui.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescGui.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDescGui.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescGui.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	D3D11_BLEND_DESC blendDescOff;
	ZeroMemory(&blendDescOff, sizeof(D3D11_BLEND_DESC));

	blendDescOff.RenderTarget[0].BlendEnable = false;
	blendDescOff.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = renderer->GetDevice()->CreateBlendState(&blendDescGui, &blendOn);
	assert(SUCCEEDED(hr));

	//END Desc for blending png files with depth//
	CD3D11_RASTERIZER_DESC rastDesc(D3D11_FILL_SOLID, D3D11_CULL_NONE, FALSE,
		D3D11_DEFAULT_DEPTH_BIAS, D3D11_DEFAULT_DEPTH_BIAS_CLAMP,
		D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS, TRUE, FALSE,
		TRUE /*this is MSAA enable*/, TRUE);

		HRESULT resultCreateRasterizer = renderer->GetDevice()->CreateRasterizerState(&rastDesc, &testState);
	assert(SUCCEEDED(resultCreateRasterizer));



	D3D11_SAMPLER_DESC testDesc;
	ZeroMemory(&testDesc, sizeof(D3D11_SAMPLER_DESC));
	testDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	//testDesc.MaxAnisotropy = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	testDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
	testDesc.MinLOD = 0.0f;
	testDesc.MaxLOD = D3D11_FLOAT32_MAX;
	testDesc.MipLODBias = 4.0f;
	testDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	testDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	testDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	
	HRESULT hr2 = renderer->GetDevice()->CreateSamplerState(&testDesc, &samplerState);
	assert(SUCCEEDED(hr2));
}

void GUIManager::AddGUIObject(GUIObject* addObj, std::string name)
{
	GUIObjects.insert({ name,addObj });
}

void GUIManager::AddGUIObject(GUIObject* addObj, std::string name, float x, float y)
{
}

void GUIManager::AddGUIObject(std::string textureName, std::string name, float x, float y)
{
}

GUIObject* GUIManager::GetGUIObject(std::string name)
{
	std::unordered_map<std::string, GUIObject*>::const_iterator got = GUIObjects.find(name);
	if (got == GUIObjects.end())
		return nullptr;
	else
		return got->second;
}

void GUIManager::RemoveGUIObject(std::string name)
{
	auto temp = GUIObjects.find(name);
	if (temp != GUIObjects.end())
		GUIObjects.erase(temp);
}

void GUIManager::Pass(Renderer* renderer, RenderTexture& inTexture, RenderTexture& outTexture)
{

	//SPRITES

	spriteBatch->Begin(dx::SpriteSortMode::SpriteSortMode_BackToFront, blendOn, samplerState, depthStencilState, testState);
	for (auto i : GUIObjects)
	{
		if(!i.second->HasGroup(GuiGroup::Font))
			i.second->Draw(spriteBatch);
	}
	spriteBatch->End();

	//FONTS

	spriteBatch->Begin(dx::SpriteSortMode::SpriteSortMode_BackToFront, nullptr, samplerState, depthStencilState, testState);
	for (auto i : GUIObjects)
	{
		if (i.second->HasGroup(GuiGroup::Font))
			i.second->Draw(spriteBatch);
	}
	spriteBatch->End();
}

void GUIManager::ChangeGuiGroup(GuiGroup state)
{
	for (auto i : GUIObjects)
	{
		if (i.second->HasGroup(state))
		{
			i.second->SetVisible(true);
		}
		else
		{
			i.second->SetVisible(false);
		}
	}
}

void GUIManager::ClearGui()
{
	for  (auto i : GUIObjects)
	{
		i.second->SetVisible(false);
	}
}

void GUIManager::UpdateAll()
{
	for (auto i : GUIObjects)
	{
		if(i.second ->GetVisible())
		i.second->Update();
	}
}


bool GUIObject::HasGroup(GuiGroup flag) const
{
	return ((int)flag & (int)group) != 0;
}

void GUIObject::AddGroup(GuiGroup flag)
{
	group = static_cast<GuiGroup>((int)group | (int)flag);
}

void GUIObject::RemoveGroup(GuiGroup flag)
{
	group = static_cast<GuiGroup>((int)group & ~((int)flag));
}


