#pragma once
#include "Buffers.h"
#include <unordered_map>
#include "DXHelper.h"
#include "ConstantBuffer.h"
#include "VirtualCamera.h"

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

	size_t RegisterPointLight(PointLightComponent* pointLight);

	PointLightComponent* GetPointLight(size_t index);
	void RemovePointLight(size_t index);
	void UpdateBuffers(dx::XMFLOAT3 eye, ID3D11DeviceContext* context);
	void Clear();

	VirtualCamera* GetShadowCamera() const { return this->shadowCamera; }
	dx::XMMATRIX GetShadowCameraTransform() const { return dx::XMLoadFloat4x4(&shadowCameraTransform); }
	void UpdateShadowCameraTransform(dx::XMFLOAT3 eye);

private:
	//cb_Scene cb_scene;
	ConstantBuffer<cb_Lights> lightBuffer;
	VirtualCamera* shadowCamera;
	dx::XMFLOAT4X4 shadowCameraTransform;

	size_t index;
	bool dirty;
	std::unordered_map<size_t, PointLightComponent*> pointLightMap;
};