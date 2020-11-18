#include "stdafx.h"
#include "LightComponent.h"
#include "LightManager.h"

LightComponent::LightComponent(UINT type, dx::XMFLOAT4 color, float range, dx::XMFLOAT3 attenuation) : type(LightType(type)), lightColor(color), range(range), attenuation(attenuation), lightDirection({ 0, -1, 1 }), spotlightAngle(.4f), enabled(false),intensity(0.2f), dirty(true) {}
LightComponent::~LightComponent() {}

void LightComponent::Initialize()
{
	LightManager::Instance().RegisterLight(this);
}

dx::XMFLOAT4 LightComponent::GetColor()
{
	return this->lightColor;
}

void LightComponent::SetColor(dx::XMFLOAT4 color)
{
	this->dirty = true;
	this->lightColor = color;
}

float LightComponent::GetRange()
{
	return this->range;
}

void LightComponent::SetRange(float range)
{
	this->dirty = true;
	this->range = range;
}

dx::XMFLOAT3 LightComponent::GetAttenuation()
{
	return this->attenuation;
}

void LightComponent::SetAttenuation(dx::XMFLOAT3 attenuation)
{
	this->dirty = true;
	this->attenuation = attenuation;
}

dx::XMFLOAT3 LightComponent::GetDirection()
{
	return this->lightDirection;
}

void LightComponent::SetDirection(dx::XMFLOAT3 direction)
{
	this->dirty = true;
	this->lightDirection = direction;
}

float LightComponent::GetSpotlightAngle()
{
	return this->spotlightAngle;
}

void LightComponent::SetSpotlightAngle(float angle)
{
	this->dirty = true;
	this->spotlightAngle = angle;
}

bool LightComponent::GetEnabled()
{
	return this->enabled;
}

void LightComponent::SetEnabled(bool enabled)
{
	this->dirty = true;
	this->enabled = enabled;
}

LightType LightComponent::GetType()
{
	return this->type;
}
void LightComponent::SetType(LightType type)
{
	this->dirty = true;
	this->type = type;
}

float LightComponent::GetIntensity()
{
	return this->intensity;
}

void LightComponent::SetIntensity(float intensity)
{
	this->dirty = true;
	this->intensity = intensity;
}
