#include "stdafx.h"
#include "PointLightComponent.h"
#include "LightManager.h"

PointLightComponent::PointLightComponent(dx::XMFLOAT4 color, float range, dx::XMFLOAT3 attenuation) : lightColor(color), range(range), attenuation(attenuation), dirty(true) {}
PointLightComponent::~PointLightComponent() {}

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
	this->dirty = true;
	this->lightColor = color;
}

float PointLightComponent::GetRange()
{
	return this->range;
}

void PointLightComponent::SetRange(float range)
{
	this->dirty = true;
	this->range = range;
}

dx::XMFLOAT3 PointLightComponent::GetAttenuation()
{
	return this->attenuation;
}

void PointLightComponent::SetAttenuation(dx::XMFLOAT3 attenuation)
{
	this->dirty = true;
	this->attenuation = attenuation;
}
