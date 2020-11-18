#include "stdafx.h"
#include "LightManager.h"
#include "PointLightComponent.h"

LightManager::LightManager()
{
	this->lightCount = 1;
	this->index = 0;
}

LightManager::~LightManager()
{
}

void LightManager::Initialize(ID3D11Device* device)
{
	//lightBuffer.Initialize(CB_LIGHT_SLOT, ShaderBindFlag::PIXEL | ShaderBindFlag::DOMAINS, device);
	the_Lights.resize(this->lightCount);
	DXHelper::CreateStructuredBuffer(device, &the_Light_srvbuffer, the_Lights.data(), sizeof(s_Light), the_Lights.size(), &the_Light_srv);
}

size_t LightManager::RegisterPointLight(PointLightComponent* pointLight)
{
	if (index < lightCount)
	{
		pointLightMap.insert({ index, pointLight });
		return this->index++;
	}
	else
		return this->index;
}

PointLightComponent* LightManager::GetPointLight(size_t index)
{
	std::unordered_map<size_t, PointLightComponent*>::const_iterator lightObject = pointLightMap.find(index);
	if (lightObject == pointLightMap.end())
		return nullptr;
	else
		return lightObject->second;
}

void LightManager::RemovePointLight(size_t index)
{
	auto temp = pointLightMap.find(index);
	if (temp != pointLightMap.end())
		pointLightMap.erase(temp);
}

void LightManager::UpdateBuffers(ID3D11DeviceContext* context, CameraComponent* camComp)
{
	bool updated = false;

	for (auto i = pointLightMap.begin(); i != pointLightMap.end() && !updated; i++)
	{
		if (i->second->IsDirty())
		{
			ForceUpdateBuffers(context,camComp);
			updated = true;
		}
	}
}

void LightManager::ForceUpdateBuffers(ID3D11DeviceContext* context, CameraComponent* camComp)
{
	
	//cb_Lights& data = lightBuffer.GetData();
	//dx::XMStoreFloat3(&data.sunDirection, dx::XMVector3Normalize(dx::XMVectorSet(0, -1, 1, 0)));
	//data.sunIntensity = 0.1f;
	//data.nrOfLights = pointLightMap.size();



	for (auto i = pointLightMap.begin(); i != pointLightMap.end(); i++)
	{
		i->second->MarkAsNotDirty();

		the_Lights[i->first].lightColor = i->second->GetColor();
		dx::XMStoreFloat4(&the_Lights[i->first].lightPosition, (i->second->GetOwner()->GetTransform().GetWorldPosition()));
		dx::XMVECTOR viewPos = dx::XMVector3TransformCoord(i->second->GetOwner()->GetTransform().GetWorldPosition(), camComp->GetViewMatrix());
		dx::XMStoreFloat4(&the_Lights[i->first].lightPositionVS, viewPos);
		the_Lights[i->first].lightDirection = i->second->GetDirection();
		the_Lights[i->first].range = i->second->GetRange();
		the_Lights[i->first].attenuation = i->second->GetAttenuation();
		the_Lights[i->first].spotlightAngle = i->second->GetSpotlightAngle();
		the_Lights[i->first].enabled = (UINT)i->second->GetEnabled();
		the_Lights[i->first].type = (UINT)i->second->GetType();
		the_Lights[i->first].intensity = i->second->GetIntensity();
	}

	DXHelper::BindStructuredBuffer(context, the_Light_srvbuffer, the_Lights.data(), 8, ShaderBindFlag::PIXEL | ShaderBindFlag::DOMAINS| ShaderBindFlag::COMPUTE, &the_Light_srv); //t8
	//lightBuffer.SetData(data);
	//lightBuffer.UpdateBuffer(context);
}

void LightManager::Clear()
{
	pointLightMap.clear();
}
