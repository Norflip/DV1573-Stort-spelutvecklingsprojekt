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
	return pointLightMap[index];
}
