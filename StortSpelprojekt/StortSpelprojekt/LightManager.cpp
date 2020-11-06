#include "stdafx.h"
#include "LightManager.h"
#include "PointLightComponent.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

void LightManager::Initialize(ID3D11Device* device)
{
	lightBuffer.Initialize(CB_LIGHT_SLOT, ShaderBindFlag::PIXEL | ShaderBindFlag::DOMAINS, device);
}

size_t LightManager::RegisterPointLight(PointLightComponent* pointLight)
{
	pointLightMap.insert({ index, pointLight });
	return this->index++;
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

void LightManager::UpdateBuffers(ID3D11DeviceContext* context)
{
	bool dirty = false;
	cb_Lights& data = lightBuffer.GetData();

	dx::XMStoreFloat3(&data.sunDirection, dx::XMVector3Normalize(dx::XMVectorSet(0, -1, 1, 0)));
	data.sunIntensity = 0.1f;
	data.nrOfPointLights = pointLightMap.size();

	for (auto i = pointLightMap.begin(); i != pointLightMap.end(); i++)
	{
		if (i->second->IsDirty())
		{
			dirty = true;
			i->second->MarkAsNotDirty();
		}

		data.pointLights[i->first].lightColor = i->second->GetColor();
		dx::XMStoreFloat3(&data.pointLights[i->first].lightPosition, (i->second->GetOwner()->GetTransform().GetWorldPosition()));
		data.pointLights[i->first].attenuation = i->second->GetAttenuation();
		data.pointLights[i->first].range = i->second->GetRange();
	}
	
	
		lightBuffer.SetData(data);
		lightBuffer.UpdateBuffer(context);
	
}

void LightManager::Clear()
{
	pointLightMap.clear();
}
