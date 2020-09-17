#pragma once
#include "DXHelper.h"

class Texture {
public:
	Texture();
	~Texture();

	bool InitializeTexture();
	bool RenderToTexture();
	void SetRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv);
	void ClearRenderTarget(ID3D11DeviceContext* context, ID3D11DepthStencilView* dsv, float r, float g, float b, float a);

	ID3D11ShaderResourceView* GetDiffuseMap() { return this->srv_DiffuseMap; }
	ID3D11ShaderResourceView* GetNormalMap() { return this->srv_DiffuseMap; }
	bool LoadDiffusemap();
	bool LoadNormalmap();

	void Shutdown();
	
private:
	HRESULT hr;
	ID3D11ShaderResourceView* srv_DiffuseMap;
	ID3D11ShaderResourceView* srv_NormalMap;

	ID3D11Texture2D* renderTargetTexture;
	ID3D11RenderTargetView* rtv;
};