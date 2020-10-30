#include "stdafx.h"
#include "Texture.h"

Texture::Texture()
{	
	sizeof(&hr, sizeof(HRESULT));
	srv = nullptr;
}

Texture::Texture(ID3D11ShaderResourceView* srv) : srv(srv)
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
}