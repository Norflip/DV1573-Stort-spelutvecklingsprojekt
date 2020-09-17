#pragma once
#include "Renderer.h"
#include "..\packages\directxtk_desktop_win10.2020.8.15.1\include\WICTextureLoader.h"

#define TEXTURE_DIFFUSE_SLOT 0
#define TEXTURE_NORMAL_SLOT 1

class Texture {
public:
	Texture(Renderer* renderer);
	~Texture();

	bool LoadTexture(ID3D11Device* device, LPCWSTR textureFilepath);
	ID3D11ShaderResourceView* GetTexture() { return this->srv; }
	ID3D11SamplerState* GetSampler() { return this->samplerState; }

	/* render to texture */
	bool RenderToTexture(ID3D11Device* device, int textureWidth, int textureHeight);
	void SetRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv);
	void ClearRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv, float r, float g, float b, float a);	
	
	void Shutdown();
	
private:
	HRESULT hr;
	ID3D11ShaderResourceView* srv;	
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* renderTargetTexture;
	ID3D11SamplerState* samplerState;
};