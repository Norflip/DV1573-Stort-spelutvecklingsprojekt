#pragma once
#include "Buffers.h"
#include <unordered_map>
#include "DXHelper.h"
#include "ConstantBuffer.h"

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
	std::vector<s_Light>& GetLightData() { return lightData; }
	void SetLightData(std::vector<s_Light>& lightData) { this->lightData = lightData; }
	size_t RegisterPointLight(PointLightComponent* pointLight);

	PointLightComponent* GetPointLight(size_t index);
	void RemovePointLight(size_t index);
	void UpdateBuffers(ID3D11DeviceContext* context, CameraComponent* camComp);
	void ForceUpdateBuffers(ID3D11DeviceContext* context, CameraComponent* camComp);
	void Clear();
	UINT GetPointLightCount(){ return (UINT)pointLightMap.size(); }
private:
	ID3D11ShaderResourceView* lightsSRV = nullptr;
	ID3D11Buffer* lightsSRVBfr = nullptr;
	//cb_Scene cb_scene;
	ConstantBuffer<cb_Lights> lightBuffer;
	size_t index;
	std::unordered_map<size_t, PointLightComponent*> pointLightMap;
	std::vector<s_Light> lightData;
};