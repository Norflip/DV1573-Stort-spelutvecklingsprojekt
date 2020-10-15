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

	data.sunDirection = dx::XMFLOAT3(0.0f, 100.0f, -45.0f);
	data.sunIntensity = 0.4f;
	data.nrOfPointLights = pointLightMap.size();

	for (auto i = pointLightMap.begin(); i != pointLightMap.end(); i++)
	{
		if (i->second->IsDirty())
		{
			dirty = true;
			i->second->MarkAsNotDirty();
		}

		data.pointLights[i->first].lightColor = i->second->GetColor();
		dx::XMStoreFloat3(&data.pointLights[i->first].lightPosition, (i->second->GetOwner()->GetTransform().GetPosition()));
		data.pointLights[i->first].attenuation = i->second->GetAttenuation();
		data.pointLights[i->first].range = i->second->GetRange();
	}
	
	if (dirty)
	{
		lightBuffer.SetData(data);
		lightBuffer.UpdateBuffer(context);
	}
}
