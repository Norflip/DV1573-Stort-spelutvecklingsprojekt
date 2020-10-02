#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

void LightManager::RegisterPointLight(PointLightComponent* pointLight, int index)
{
	pointLightMap.insert({ index, pointLight });
}

PointLightComponent* LightManager::GetPointLight(int index)
{
	std::unordered_map<int, PointLightComponent*>::const_iterator lightObject = pointLightMap.find(index);
	if (lightObject == pointLightMap.end())
		return nullptr;
	else
		return lightObject->second;
}

void LightManager::RemovePointLight(int index)
{
	auto temp = pointLightMap.find(index);
	if (temp != pointLightMap.end())
		pointLightMap.erase(temp);
}
