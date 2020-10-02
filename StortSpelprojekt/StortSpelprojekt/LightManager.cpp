#include "LightManager.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

void LightManager::RegisterPointLight(PointLightComponent* pointLight/*, int index*/)
{
	int index; //tremporär tills jag kommer op vad som faktiskt ska vara i insert funktionen
	pointLightMap.insert({ index, pointLight });
}

bool LightManager::getPointLight(int index)
{
	std::unordered_map<int, PointLightComponent*>::const_iterator lightObject = pointLightMap.find(index);
	if (lightObject == pointLightMap.end())
		return false;
	else
		//return true;
		return pointLightMap[index];
}
