#pragma once
#include "PointLightComponent.h"
#include <unordered_map>
class LightManager
{
public:

	LightManager();
	LightManager(Renderer* renderer);
	virtual ~LightManager();


	static LightManager& Instance() // singleton
	{
		static LightManager instance;
		return instance;
	}

	LightManager(LightManager const&) = delete;
	void operator=(LightManager const&) = delete;

	bool IsDirty() { return dirty; }; // kollar ifall den har skräpväreden
	size_t RegisterPointLight(PointLightComponent* pointLight);

	PointLightComponent* GetPointLight(size_t index);
	void RemovePointLight(size_t index);

private:

	PointLightComponent* pointLightComponent;
	size_t index;
	bool dirty;
	std::unordered_map<size_t, PointLightComponent*> pointLightMap;

};