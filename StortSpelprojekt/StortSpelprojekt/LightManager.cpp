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
	DXHelper::CreateConstBuffer(device, &light_cbuffer, &cb_light, sizeof(cb_light));

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

void LightManager::UpdateBuffers(ID3D11DeviceContext* context, dx::XMVECTOR camPos)
{
	cb_light.sunDirection = dx::XMFLOAT3(0.0f, 100.0f, -45.0f);
	cb_light.sunIntensity = 0.4f;
	for (auto i = pointLightMap.begin(); i != pointLightMap.end(); i++)
	{
		cb_light.pointLights[i->first].lightColor = dx::XMFLOAT4(0.5f, 0.5f, 0.5f, 1.0f);
		dx::XMStoreFloat3(&cb_light.pointLights[i->first].lightPosition, (GetPointLight(i->first)->GetOwner()->GetTransform().GetPosition()));
		cb_light.pointLights[i->first].attenuation = dx::XMFLOAT3(1.0f, 0.02f, 0.0f);
		cb_light.pointLights[i->first].range = 25.0f;
	}

	cb_light.nrOfPointLights = 2;
	dx::XMStoreFloat3(&cb_light.cameraPosition, camPos);
	DXHelper::BindConstBuffer(context, light_cbuffer, &cb_light, CB_SCENE_SLOT, ShaderBindFlag::DOMAINS);
	DXHelper::BindConstBuffer(context, light_cbuffer, &cb_light, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);
}