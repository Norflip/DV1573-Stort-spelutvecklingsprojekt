#include "stdafx.h"
#include "DepthPass.h"



void DepthPass::Init(ID3D11Device* device, UINT width, UINT height)
{
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
	D3D11_TEXTURE2D_DESC textureDesc = {};
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};

	ID3D11Texture2D* depthTex2D = nullptr;

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HRESULT HR = device->CreateTexture2D(&textureDesc, NULL, &depthTex2D);
	assert(SUCCEEDED(HR));

	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	depthStencilViewDesc.Flags = 0;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	// Create the depth stencil view.
	HR = device->CreateDepthStencilView(depthTex2D, &depthStencilViewDesc, &depthDSV);
	assert(SUCCEEDED(HR));

	shaderResourceViewDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceViewDesc.Texture2D.MipLevels = textureDesc.MipLevels;

	HR = device->CreateShaderResourceView(depthTex2D, &shaderResourceViewDesc, &depthSRV); //Create a shader resource view for the depth map.
	assert(SUCCEEDED(HR));


	depthTex2D->Release();
}

void DepthPass::BindDSV(ID3D11DeviceContext* context)
{

	context->OMSetRenderTargets(0, 0, depthDSV);
	context->ClearDepthStencilView(depthDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void DepthPass::BindNull(ID3D11DeviceContext* context)
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	ID3D11DepthStencilView* nullDSV = nullptr;
	ID3D11RenderTargetView* nullRTV = nullptr;
	context->OMSetRenderTargets(1, &nullRTV, nullDSV);
	context->CSSetShaderResources(1, 1, &nullSRV);
}
