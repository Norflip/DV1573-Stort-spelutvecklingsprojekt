#include "Skybox.h"

Skybox::Skybox(ID3D11Device* device, ID3D11DeviceContext* context, Object* object)
{
	this->object = object;

	skyboxShader.SetPixelShader(L"Shaders/Sky_ps.hlsl");
	skyboxShader.SetVertexShader(L"Shaders/Sky_vs.hlsl");
	skyboxShader.Compile(device);

	skyboxMesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/Skybox.ZWEB", device)[0];
	skyboxMaterial = Material(skyboxShader);
	
	// Load all textures 
	LoadAllTextures(context, device);

	// Add components
	this->object->AddComponent<MeshComponent>(skyboxMesh, skyboxMaterial);
	
	
	this->object->GetTransform().SetScale(dx::XMVECTOR(dx::XMVectorSet(10,10,10, 1.0f)));

	object->GetComponent<MeshComponent>()->GetMaterial().GetTexture().SetTexture(srvs[1]);
	float as = 0;
}

Skybox::~Skybox()
{
}

void Skybox::LoadAllTextures(ID3D11DeviceContext* context, ID3D11Device* device)
{
	hr = dx::CreateWICTextureFromFile(device, L"Textures/Day_Sunless.png", nullptr, &srv);
	if (FAILED(hr))	
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);
	
	srvs.push_back(srv);
	
	hr = dx::CreateWICTextureFromFile(device, L"Textures/Night_Moonless.png", nullptr, &srv);
	if (FAILED(hr))	
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);
	
	srvs.push_back(srv);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/Sunrise.png", nullptr, &srv);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);

	srvs.push_back(srv);

	hr = dx::CreateWICTextureFromFile(device, L"Textures/sunset.png", nullptr, &srv);
	if (FAILED(hr))
		MessageBox(0, L"Failed to 'Load DDS Texture' - (skymap.dds).", L"Graphics scene Initialization Message", MB_ICONERROR);

	srvs.push_back(srv);
	
	// Set first texture
	texture.SetTexture(srvs[2]);	

	skyboxMaterial.SetTexture(texture, TEXTURE_DIFFUSE_SLOT, ShaderBindFlag::PIXEL);
	skyboxMaterial.SetSamplerState(device, D3D11_TEXTURE_ADDRESS_WRAP, D3D11_FILTER_MIN_MAG_MIP_LINEAR);			
}