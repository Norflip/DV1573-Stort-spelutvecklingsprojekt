#include "stdafx.h"

#include "Shader.h"
#include "Skybox.h"

Skybox::Skybox(ID3D11Device* device, ID3D11DeviceContext* context, Shader* shader)
{
	this->object = new Object("skybox");

	skyboxShader = shader;

	skyboxMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/Skybox.ZWEB", device)[0];
	skyboxMaterial = new Material(skyboxShader);
	
	// Load all textures 
	LoadAllTextures(context, device);

	// Add components
	this->object->AddComponent<MeshComponent>(skyboxMesh, skyboxMaterial);
		
	this->object->GetTransform().SetScale(dx::XMVECTOR(dx::XMVectorSet(20,20,20, 1.0f)));	
}

Skybox::~Skybox()
{
	if (object)
	{
		delete object;
		object = nullptr;
	}
}

void Skybox::LoadAllTextures(ID3D11DeviceContext* context, ID3D11Device* device)
{
	hr = dx::CreateWICTextureFromFile(device, L"Textures/Day.png", nullptr, &srv);
	if (FAILED(hr))	
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);
	
	srvs.push_back(srv);
	
	hr = dx::CreateWICTextureFromFile(device, L"Textures/Sunset.png", nullptr, &srv);
	if (FAILED(hr))	
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);
	
	srvs.push_back(srv);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/Night.png", nullptr, &srv);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);

	srvs.push_back(srv);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/End.png", nullptr, &srv);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);

	srvs.push_back(srv);
	

	// Set first texture
	texture.SetShaderResourceView(srvs[0]);		
	skyboxMaterial->SetTexture(texture, TEXTURE_DIFFUSE2_SLOT, ShaderBindFlag::PIXEL);

	texture2.SetShaderResourceView(srvs[1]);
	skyboxMaterial->SetTexture(texture2, TEXTURE_DIFFUSE3_SLOT, ShaderBindFlag::PIXEL);

	texture3.SetShaderResourceView(srvs[2]);
	skyboxMaterial->SetTexture(texture3, TEXTURE_DIFFUSE4_SLOT, ShaderBindFlag::PIXEL);

	texture4.SetShaderResourceView(srvs[3]);
	skyboxMaterial->SetTexture(texture4, TEXTURE_DIFFUSE5_SLOT, ShaderBindFlag::PIXEL);

	skyboxMaterial->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
}