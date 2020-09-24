#include "Skybox.h"

Skybox::Skybox(ID3D11Device* device, Object* object)
{
	this->object = object;

	skyboxShader.SetPixelShader(L"Shaders/Sky_ps.hlsl");
	skyboxShader.SetVertexShader(L"Shaders/Sky_vs.hlsl");
	skyboxShader.Compile(device);

	skyboxMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/Skybox.ZWEB", device)[0];
	skyboxMaterial = Material(skyboxShader);

	this->object->AddComponent<MeshComponent>(skyboxMesh, skyboxMaterial);

	dx::XMFLOAT3 scale = this->skyboxMesh.GetS();
	this->object->GetTransform().SetScale(dx::XMVECTOR(dx::XMVectorSet(10,10,10, 1.0f)));
	
}

Skybox::~Skybox()
{
}

void Skybox::LoadDDS(ID3D11DeviceContext* context, ID3D11Device* device, LPCWSTR textureFilename)
{
	ID3D11ShaderResourceView* srv; 

	hr = dx::CreateDDSTextureFromFileEx(device, context, textureFilename, 0, D3D11_USAGE_DEFAULT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE, 0, D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE, false, nullptr, &srv); 
	if (FAILED(hr)) {
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);		
	}

	texture.SetTexture(srv);

	skyboxMaterial.SetTexture(texture, TEXTURE_CUBE_SLOT, ShaderBindFlag::PIXEL);
	skyboxMaterial.SetSamplerState(device, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);
}