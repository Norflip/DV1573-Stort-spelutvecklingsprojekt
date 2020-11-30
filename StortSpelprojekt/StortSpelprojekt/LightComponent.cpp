#include "stdafx.h"
#include "LightComponent.h"
#include "LightManager.h"

LightComponent::LightComponent(UINT type, dx::XMFLOAT4 color, float range) 
{
	this->type = (LightType)type; 
	this->lightColor = color;
	this->range = range;
	
	this->lightDirection = dx::XMFLOAT3({ 0, -1, 0 });
	this->spotlightAngle = 15.0f;
	this->enabled = false;
	this->intensity = 0.5f;
	this->dirty=true;
	this->elapsedTime = 0.f;
}
LightComponent::LightComponent(LightType type, dx::XMFLOAT4 color, float range) 
{
	this->type = type;
	this->lightColor = color;
	this->range = range;

	this->lightDirection = dx::XMFLOAT3({ 0, -1, 0 });
	this->spotlightAngle = 15.0f;
	this->enabled = false;
	this->intensity = 0.5f;
	this->dirty = true;
	this->elapsedTime = 0.f;
}

LightComponent::~LightComponent() 
{

}

void LightComponent::Initialize()
{
	LightManager::Instance().RegisterLight(this);
}

dx::XMFLOAT4 LightComponent::GetColor()const
{
	return this->lightColor;
}

void LightComponent::SetColor(const dx::XMFLOAT4 & color)
{
	this->dirty = true;
	this->lightColor = color;
}

float LightComponent::GetRange()const
{
	return this->range;
}

void LightComponent::SetRange(const float & range)
{
	this->dirty = true;
	this->range = range;
}

//dx::XMFLOAT3 LightComponent::GetAttenuation()
//{
//	return this->attenuation;
//}
//
//void LightComponent::SetAttenuation(dx::XMFLOAT3 attenuation)
//{
//	this->dirty = true;
//	this->attenuation = attenuation;
//}

dx::XMFLOAT3 LightComponent::GetDirection()const
{
	return this->lightDirection;
}

void LightComponent::SetDirection(const dx::XMFLOAT3 & direction)
{
	this->dirty = true;
	this->lightDirection = direction;
}

float LightComponent::GetSpotlightAngle()const
{
	return this->spotlightAngle;
}

void LightComponent::SetSpotlightAngle(const float & angle)
{
	this->dirty = true;
	this->spotlightAngle = angle;
}

bool LightComponent::GetEnabled()const
{
	return this->enabled;
}

void LightComponent::SetEnabled(const bool & enabled)
{
	this->dirty = true;
	this->enabled = enabled;
}

LightType LightComponent::GetType()const
{
	return this->type;
}
void LightComponent::SetType(const LightType & type)
{
	this->dirty = true;
	this->type = type;
}

float LightComponent::GetIntensity()const
{
	return this->intensity;
}

void LightComponent::SetIntensity(const float & intensity)
{
	this->dirty = true;
	this->intensity = intensity;
}
