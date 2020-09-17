#include "Texture.h"

Texture::Texture(Renderer* renderer)
{
	hr = 0;
	srv = 0;
	rtv = 0;
	renderTargetTexture = 0;
	samplerState = renderer->GetSampler();		
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
	if (samplerState) {
		samplerState->Release();
		samplerState = 0;
	}
	if (srv) {
		srv->Release();
		srv = 0;
	}

	if (renderTargetTexture) {
		renderTargetTexture->Release();
		renderTargetTexture = 0;
	}

	if (rtv) {
		rtv->Release();
		rtv = 0;
	}
}