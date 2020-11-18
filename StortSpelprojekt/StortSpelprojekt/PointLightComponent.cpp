#include "stdafx.h"
#include "PointLightComponent.h"
#include "LightManager.h"

PointLightComponent::PointLightComponent(UINT type, dx::XMFLOAT4 color, float range, dx::XMFLOAT3 attenuation) : type(LightType(type)), lightColor(color), range(range), attenuation(attenuation), lightDirection({ 0, -1, 1 }), spotlightAngle(.4f), enabled(false),intensity(0.2f), dirty(true) {}
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

dx::XMFLOAT3 PointLightComponent::GetDirection()
{
	return this->lightDirection;
}

void PointLightComponent::SetDirection(dx::XMFLOAT3 direction)
{
	this->dirty = true;
	this->lightDirection = direction;
}

float PointLightComponent::GetSpotlightAngle()
{
	return this->spotlightAngle;
}

void PointLightComponent::SetSpotlightAngle(float angle)
{
	this->dirty = true;
	this->spotlightAngle = angle;
}

bool PointLightComponent::GetEnabled()
{
	return this->enabled;
}

void PointLightComponent::SetEnabled(bool enabled)
{
	this->dirty = true;
	this->enabled = enabled;
}

LightType PointLightComponent::GetType()
{
	return this->type;
}
void PointLightComponent::SetType(LightType type)
{
	this->dirty = true;
	this->type = type;
}

float PointLightComponent::GetIntensity()
{
	return this->intensity;
}

void PointLightComponent::SetIntensity(float intensity)
{
	this->dirty = true;
	this->intensity = intensity;
}
