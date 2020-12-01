#include "stdafx.h"
#include "Texture.h"
#include "Engine.h"

Texture::Texture() 
	: srv(nullptr), buffer(nullptr), width(-1), height(-1), channels(-1)
{	
}

Texture::Texture(ID3D11ShaderResourceView* srv)
	: srv(srv), buffer(nullptr), width(-1), height(-1), channels(-1)
{
}

Texture::Texture(ID3D11ShaderResourceView* srv, unsigned char* buffer, size_t width, size_t height, size_t channels)
	: srv(srv), buffer(buffer), width(width), height(height), channels(channels)
{
}

Texture::~Texture()
{		
	RELEASE(srv);
}

Texture* Texture::CreateFromBuffer(unsigned char* buffer, size_t width, size_t height, size_t channels, DXGI_FORMAT format, ID3D11Device* device)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

	D3D11_SUBRESOURCE_DATA textureResourceData;
	ZeroMemory(&textureResourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	textureResourceData.pSysMem = buffer;
	textureResourceData.SysMemPitch = width * 4;


	ID3D11Texture2D* texture;
	ZeroMemory(&texture, sizeof(ID3D11Texture2D));
	device->CreateTexture2D(&textureDesc, &textureResourceData, &texture);

	ID3D11ShaderResourceView* srv = nullptr;

	if (texture != nullptr)
	{
		ZeroMemory(&srv, sizeof(ID3D11ShaderResourceView));
		device->CreateShaderResourceView(texture, nullptr, &srv);

		texture->Release();
		texture = nullptr;
	}

	return new Texture(srv, buffer, width, height, channels);
}

Texture* Texture::LoadTexture(ID3D11Device* device, LPCWSTR textureFilepath)
{
#if LOAD_FROM_DLL

	std::wstring wstr(textureFilepath);
	std::string path (wstr.begin(), wstr.end());
	size_t size = -1;
	unsigned char* data = Engine::Instance->GetResources()->DLLGetTextureData(path, size);

	Texture* t = FromMemory(device, data, size);

		return t;
#else
	ID3D11ShaderResourceView* srv;
	HRESULT hr = DirectX::CreateWICTextureFromFile(device, textureFilepath, nullptr, &srv);
	assert(SUCCEEDED(hr));
	return new Texture(srv);
#endif
}

Texture* Texture::FromMemory(ID3D11Device* device, uint8_t* data, size_t dataSize)
{
	ID3D11ShaderResourceView* srv;

	DirectX::CreateWICTextureFromMemory(device, data, dataSize, nullptr, &srv);
	return new Texture(srv);
}
