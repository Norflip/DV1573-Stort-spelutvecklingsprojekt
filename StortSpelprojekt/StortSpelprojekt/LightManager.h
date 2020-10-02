#pragma once
#include "PointLightComponent.h"
#include <unordered_map>
class LightManager
{
public:

	LightManager();
	virtual ~LightManager();


	static LightManager& Instance() // singleton
	{
		static LightManager instance;
		return instance;
	}

	LightManager(LightManager const&) = delete;
	void operator=(LightManager const&) = delete;

	bool IsDirty() { return dirty; }; // kollar ifall den har skräpväreden
	void RegisterPointLight(PointLightComponent* pointLight/*, int index*/);

	bool getPointLight(int index);

private:

	bool dirty;
	std::unordered_map<int, PointLightComponent*> pointLightMap;

};