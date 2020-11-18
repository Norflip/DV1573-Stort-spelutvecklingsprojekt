#pragma once
#include "Buffers.h"
#include <unordered_map>
#include "DXHelper.h"
#include "ConstantBuffer.h"

class LightComponent;

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
	size_t RegisterLight(LightComponent* pointLight);

	LightComponent* GetLight(size_t index);
	void RemoveLight(size_t index);
	void UpdateBuffers(ID3D11DeviceContext* context, CameraComponent* camComp);
	void ForceUpdateBuffers(ID3D11DeviceContext* context, CameraComponent* camComp);
	void Clear();
	UINT GetLightCount(){ return (UINT)lightMap.size(); }
private:
	ID3D11ShaderResourceView* lightsSRV = nullptr;
	ID3D11Buffer* lightsSRVBfr = nullptr;
	//cb_Scene cb_scene;
	//ConstantBuffer<cb_Lights> lightBuffer;
	//
	std::vector<s_Light> the_Lights;
	ID3D11Buffer* the_Light_srvbuffer = 0;
	ID3D11ShaderResourceView* the_Light_srv = 0;
	UINT lightCount; //total lights


	size_t index;
	std::unordered_map<size_t, LightComponent*> lightMap;
	std::vector<s_Light> lightData;
};