#pragma once
#include "..\packages\directxtk_desktop_win10.2020.8.15.1\include\WICTextureLoader.h"

#define TEXTURE_DIFFUSE_SLOT 0
#define TEXTURE_NORMAL_SLOT 1

#define TEXTURE_DIFFUSE2_SLOT 2
#define TEXTURE_DIFFUSE3_SLOT 3
#define TEXTURE_DIFFUSE4_SLOT 4
#define TEXTURE_DIFFUSE5_SLOT 5

#define TEXTURE_ALPHA_SLOT 1
#define TEXTURE_CUBE_SLOT 2


class Texture {
public:
	Texture();
	Texture(ID3D11ShaderResourceView* srv);
	~Texture();

	bool LoadTexture(ID3D11Device* device, LPCWSTR textureFilepath);

	void SetTexture(ID3D11ShaderResourceView* srv) { this->srv = srv; }
	ID3D11ShaderResourceView* GetTexture() const { return this->srv; }

	void SetRenderTarget(ID3D11RenderTargetView* rtv) { this->rtv = rtv; }
	ID3D11RenderTargetView* GetRtv() { return this->rtv; }

	void Shutdown();
	
private:
	HRESULT hr;
	ID3D11ShaderResourceView* srv;	
	ID3D11RenderTargetView* rtv;
};