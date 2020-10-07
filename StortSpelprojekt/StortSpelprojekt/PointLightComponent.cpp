#include "PointLightComponent.h"
#include "LightManager.h"

PointLightComponent::PointLightComponent()
{
	this->lightColor = { 1, 1, 1, 1 };
	this->range = 25;
	this->attenuation = { 1.0f, 0.02f, 0.0f };

	//DXHelper::CreateConstBuffer(device, &lightBuffer, &lightData, sizeof(lightData));
	//DXHelper::BindConstBuffer(context, lightBuffer, &lightData, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);
	//screenQuadShader.SetPixelShader(L"Shaders/ScreenQuad_ps.hlsl");
}

PointLightComponent::PointLightComponent(dx::XMFLOAT4 lightColor, float range, dx::XMFLOAT3 attenuation, ID3D11Device* device, ID3D11DeviceContext* context)
{
	/*this->lightColor = { 1, 1, 1, 1 };
	this->range = 25;
	this->attenuation = { 1.0f, 0.02f, 0.0f };*/

	//DXHelper::CreateConstBuffer(device, &lightBuffer, &lightData, sizeof(lightData));
	//DXHelper::BindConstBuffer(context, lightBuffer, &lightData, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);
	//screenQuadShader.SetPixelShader(L"Shaders/ScreenQuad_ps.hlsl");

}

PointLightComponent::~PointLightComponent()
{
}

void PointLightComponent::Update(const float& deltaTime)
{
	this->elapsedTime += deltaTime;

	dx::XMStoreFloat3(&this->lightPosition, GetOwner()->GetTransform().GetPosition());



	//DXHelper::BindConstBuffer(context, light_cbuffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);

}

void PointLightComponent::Initialize()
{
	LightManager::Instance().RegisterPointLight(this);
}

dx::XMFLOAT4 PointLightComponent::GetColor()
{
	return this->lightColor;
}

void PointLightComponent::SetColor(dx::XMFLOAT4 color)
{
	this->lightColor = color;
}

float PointLightComponent::GetRange()
{
	return this->range;
}

void PointLightComponent::SetRange(float range)
{
	this->range = range;
}

dx::XMFLOAT3 PointLightComponent::GetAttenuation()
{
	return this->attenuation;
}

void PointLightComponent::SetAttenuation(dx::XMFLOAT3 attenuation)
{
	this->attenuation = attenuation;
}
