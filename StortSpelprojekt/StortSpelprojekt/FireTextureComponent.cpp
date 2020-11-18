#include "stdafx.h"
#include "FireTextureComponent.h"

FireTextureComponent::FireTextureComponent(Renderer* renderer, Shader* shader)
{
	this->renderer = renderer;
	this->fireShader = shader;
	this->meshMaterial = new Material(fireShader);
	this->quadMatrix = dx::XMMatrixIdentity();

	quadMaterial = ZWEBLoader::LoadMaterials("Models/Quad.ZWEB", fireShader, renderer->GetDevice());
	quadMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/Quad.ZWEB", renderer->GetDevice());
}

FireTextureComponent::~FireTextureComponent()
{
}

void FireTextureComponent::InitializeFire(ID3D11Device* device)
{
}

void FireTextureComponent::Shutdown()
{
}

void FireTextureComponent::Update(const float& deltaTime)
{
}

void FireTextureComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
}

void FireTextureComponent::LoadTextures(ID3D11Device* device)
{
	HRESULT hr = dx::CreateWICTextureFromFile(device, L"Textures/fireAlpha.png", nullptr, &alphaTexture);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load WIC Texture'", L"Graphics scene Initialization Message", MB_ICONERROR);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/fireTexture.png", nullptr, &fireTexture);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load WIC Texture'", L"Graphics scene Initialization Message", MB_ICONERROR);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/fireNoise.png", nullptr, &noiseTexture);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load WIC Texture'", L"Graphics scene Initialization Message", MB_ICONERROR);

	/* Mat info */
	quadMaterial[0]->SetTexture(new Texture(fireTexture), TEXTURE_DIFFUSE_SLOT, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetTexture(new Texture(noiseTexture), TEXTURE_DIFFUSE2_SLOT, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetTexture(new Texture(alphaTexture), TEXTURE_DIFFUSE3_SLOT, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
	quadMaterial[0]->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_CLAMP, device), 0, ShaderBindFlag::PIXEL);

}

void FireTextureComponent::InitializeBuffers(ID3D11Device* device)
{
}
