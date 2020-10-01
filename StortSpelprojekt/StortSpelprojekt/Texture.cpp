#include "Texture.h"

Texture::Texture()
{	
	srv = nullptr;
	rtv = nullptr;
}

Texture::Texture(ID3D11ShaderResourceView* srv) : srv(srv), rtv(nullptr)
{
}

Texture::~Texture()
{
	
}

bool Texture::LoadTexture(ID3D11Device* device, LPCWSTR textureFilepath)
{
	hr = DirectX::CreateWICTextureFromFile(device, textureFilepath, nullptr, &srv);
	if (FAILED(hr))
		return false;

	return true;
}

void Texture::Shutdown()
{	
	if (srv) {
		srv->Release();
		srv = 0;
	}	

	if (rtv) {
		rtv->Release();
		rtv = 0;
	}
}