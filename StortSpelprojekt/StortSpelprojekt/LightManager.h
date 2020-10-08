#pragma once
#include "Buffers.h"
#include <unordered_map>
#include "DXHelper.h"
class PointLightComponent;
class LightManager
{
public:

	LightManager();
	virtual ~LightManager();
	
	void Initialize(ID3D11Device* device);

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
	void UpdateBuffers(ID3D11DeviceContext* context, dx::XMVECTOR camPos);

private:

	//cb_Scene cb_scene;
	ID3D11Buffer* light_cbuffer;
	cb_Lights cb_light;
	size_t index;
	bool dirty;
	std::unordered_map<size_t, PointLightComponent*> pointLightMap;
};