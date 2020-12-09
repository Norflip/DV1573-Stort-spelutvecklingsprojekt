#include "stdafx.h"
#include "SkyboxRenderPass.h"

SkyboxRenderPass::SkyboxRenderPass(int priority, ResourceManager* resources)
	: RenderPass(priority, RenderPass::PassType::PRE_PASS), resources(resources), material(nullptr), mesh(nullptr)
{
	scaleRot = dx::XMMatrixScaling(-10.0f, -10.0f, -10.0f) * dx::XMMatrixRotationRollPitchYaw(0, 0, 180.0f * Math::ToRadians);
}

SkyboxRenderPass::~SkyboxRenderPass()
{
	delete material;
	delete mesh;

	for (size_t i = 0; i < 4; i++)
		delete textures[i];
}

void SkyboxRenderPass::m_Initialize(ID3D11Device* device)
{
	this->mesh = ZWEBLoader::LoadMeshes(ZWEBLoadType::NoAnimation, "Models/Props/Skybox.ZWEB", device)[0];
	Shader* shader = resources->GetShaderResource("skyboxShader");
	this->material = new Material(shader);
	LoadTextures(device);
}
void SkyboxRenderPass::Pass(Renderer* renderer, CameraComponent* camera, RenderTexture& inTexture, RenderTexture& outTexture)
{
	dx::XMVECTOR eye = camera->GetOwner()->GetTransform().GetWorldPosition();
	dx::XMMATRIX transformation = scaleRot * dx::XMMatrixTranslationFromVector(eye);
	renderer->Draw(mesh, material, transformation, false);
}

void SkyboxRenderPass::LoadTextures(ID3D11Device* device)
{
	const LPCWSTR TEXTURE_PATHS[] = {
		L"Textures/Day.png",
		L"Textures/Sunset.png",
		L"Textures/Night.png",
		L"Textures/End.png"
	};

	const size_t DIFFUSE_START_SLOT = 2;

	for (size_t i = 0; i < 4; i++)
	{
		textures[i] = Texture::LoadTexture(device, TEXTURE_PATHS[i]);
		material->SetTexture(textures[i], DIFFUSE_START_SLOT + i, ShaderBindFlag::PIXEL);
	}

	material->SetSampler(DXHelper::CreateSampler(D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP, device), 0, ShaderBindFlag::PIXEL);
}
