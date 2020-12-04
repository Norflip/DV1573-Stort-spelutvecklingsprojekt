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

Texture* Texture::CreateRandom1DTexture(ID3D11Device* device)
{
	ID3D11Texture1D* random;
	DirectX::XMFLOAT4* randomValues = new DirectX::XMFLOAT4[1024];
	for (int i = 0; i < 1024; ++i)
	{
		randomValues[i].x = RandomFloat(-1, 1);
		randomValues[i].y = RandomFloat(-1, 1);
		randomValues[i].z = RandomFloat(-1, 1);
		randomValues[i].w = RandomFloat(-1, 1);
	}

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = randomValues;
	initData.SysMemPitch = 1024 * sizeof(DirectX::XMFLOAT4);
	initData.SysMemSlicePitch = 0;

	D3D11_TEXTURE1D_DESC texDesc = {};
	texDesc.Width = 1024;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	texDesc.MipLevels = 1;

	HRESULT hr = device->CreateTexture1D(&texDesc, &initData, &random);
	assert(SUCCEEDED(hr));

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc = {};
	viewDesc.Format = texDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
	viewDesc.Texture1D.MipLevels = texDesc.MipLevels;
	viewDesc.Texture1D.MostDetailedMip = 0;

	hr = device->CreateShaderResourceView(random, &viewDesc, &srv);
	assert(SUCCEEDED(hr));
	delete[] randomValues;

	return new Texture(srv);
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
	// vi kör på std::string då den har en intern hash funktion för unordered_map
	std::wstring tmp(textureFilepath);
	std::string key(tmp.begin(), tmp.end());

	Texture* texture = Engine::Instance->GetResources()->GetMissingTexture();
	auto found = mTextureCache.find(key);
	if (found != mTextureCache.end())
	{
		std::cout << key << " from cache" << std::endl;

		return found->second;
	}
	else
	{
		ID3D11ShaderResourceView* srv;
		HRESULT hr = DirectX::CreateWICTextureFromFile(device, textureFilepath, nullptr, &srv);

		if (SUCCEEDED(hr))
		{
			texture = new Texture(srv);
		}
		else
		{
			std::cout << "Missing texture: " << textureFilepath << std::endl;
		}

		if (texture != nullptr)
		{
			mTextureCache.insert({ key, texture });
		}
	}

	return texture;
}

float Texture::RandomFloat(float a, float b)
{
	float random = ((float)rand()) / (float)RAND_MAX;
	float diff = b - a;
	float r = random * diff;
	return a + r;
}
