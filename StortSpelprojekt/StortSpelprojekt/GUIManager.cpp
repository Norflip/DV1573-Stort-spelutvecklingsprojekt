#include "GUIManager.h"

GUIManager::GUIManager(Renderer* renderer, int priority): RenderPass(priority, RenderPass::PassType::UI_OVERLAY), renderer(renderer)
{
	spriteBatch = new DirectX::SpriteBatch(this->renderer->GetContext());
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

	ID3D11DepthStencilState* m_depthStencilState;
	renderer->GetDevice()->CreateDepthStencilState(&dsDesc, &m_depthStencilState);


	HRESULT hr;
	D3D11_BLEND_DESC blendDescOn;
	ZeroMemory(&blendDescOn, sizeof(D3D11_BLEND_DESC));



	blendDescOn.RenderTarget[0].BlendEnable = TRUE;
	blendDescOn.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDescOn.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDescOn.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDescOn.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDescOn.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDescOn.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	D3D11_BLEND_DESC blendDescOff;
	ZeroMemory(&blendDescOff, sizeof(D3D11_BLEND_DESC));

	blendDescOff.RenderTarget[0].BlendEnable = false;
	blendDescOff.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	ID3D11BlendState* blendOn;
	hr = renderer->GetDevice()->CreateBlendState(&blendDescOn, &blendOn);
	assert(SUCCEEDED(hr));


	spriteBatch->Begin(dx::SpriteSortMode::SpriteSortMode_Deferred, blendOn,nullptr, m_depthStencilState);
	for (auto i : GUIObjects)
		i.second->Draw(spriteBatch);
	spriteBatch->End();
}

void GUIManager::UpdateAll()
{
	for (auto i : GUIObjects)
	{
		i.second->Update();
	}
}
