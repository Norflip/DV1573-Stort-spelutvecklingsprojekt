#include "stdafx.h"
#include "LightManager.h"
#include "PointLightComponent.h"

LightManager::LightManager()
{
	shadowCamera = new VirtualCamera(VirtualCamera::CameraMode::ORTHOGRAPHIC, 64, 64, 0.01f, 1000.0f);
}

LightManager::~LightManager()
{
	delete shadowCamera;
}

void LightManager::Initialize(ID3D11Device* device)
{
	lightBuffer.Initialize(CB_LIGHT_SLOT, ShaderBindFlag::PIXEL | ShaderBindFlag::DOMAINS, device);
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

void LightManager::UpdateBuffers(dx::XMFLOAT3 eye, ID3D11DeviceContext* context)
{
	bool dirty = false;
	cb_Lights& data = lightBuffer.GetData();
	dx::XMVECTOR sunDirection = dx::XMVectorSet(0, -1, 1, 0);
	UpdateShadowCameraTransform(eye);

	dx::XMMATRIX shadowView = shadowCamera->GetViewMatrix(dx::XMLoadFloat4x4(&shadowCameraTransform));
	dx::XMMATRIX shadowProjection = shadowCamera->GetProjectionMatrix();

	dx::XMStoreFloat3(&data.sunDirection, dx::XMVector3Normalize(sunDirection));

	dx::XMStoreFloat4x4(&data.shadowView, DirectX::XMMatrixTranspose(shadowView));
	dx::XMStoreFloat4x4(&data.shadowProjection, DirectX::XMMatrixTranspose(shadowProjection));
	dx::XMStoreFloat4x4(&data.shadowVP, DirectX::XMMatrixTranspose(shadowView * shadowProjection));

	data.sunIntensity = 0.4f;
	data.nrOfPointLights = pointLightMap.size();

	for (auto i = pointLightMap.begin(); i != pointLightMap.end(); i++)
	{
		if (i->second->IsDirty())
		{
			dirty = true;
			i->second->MarkAsNotDirty();
		}

		data.pointLights[i->first].lightColor = i->second->GetColor();
		dx::XMStoreFloat3(&data.pointLights[i->first].lightPosition, (i->second->GetOwner()->GetTransform().GetWorldPosition()));
		data.pointLights[i->first].attenuation = i->second->GetAttenuation();
		data.pointLights[i->first].range = i->second->GetRange();
	}


	lightBuffer.SetData(data);
	lightBuffer.UpdateBuffer(context);

}

void LightManager::Clear()
{
	pointLightMap.clear();
}

void LightManager::UpdateShadowCameraTransform(dx::XMFLOAT3 eye)
{
	const float cameraOffset = 30.0f;
	dx::XMVECTOR sunDirection = dx::XMVectorSet(1, -1, 1, 0);
	dx::XMVECTOR focus = dx::XMLoadFloat3(&eye);
	
	dx::XMVECTOR positionOffset = DirectX::XMVectorAdd(focus, DirectX::XMVectorScale(DirectX::XMVector3Normalize(sunDirection), cameraOffset * -1));

	dx::XMFLOAT3 offset;
	dx::XMStoreFloat3(&offset, DirectX::XMVectorSubtract(focus, positionOffset));

	float yaw = atan2f(offset.x, offset.z);
	float pitch = atan2f(sqrtf(offset.z * offset.z + offset.x * offset.x), offset.y);
	dx::XMVECTOR rotation = dx::XMQuaternionRotationRollPitchYaw(pitch, yaw, 0.0f);

	dx::XMFLOAT3 tmp;
	dx::XMStoreFloat3(&tmp, positionOffset);
	DShape::DrawSphere(tmp, 3.0f, { 0,1,1 });

	dx::XMStoreFloat4x4(&shadowCameraTransform, dx::XMMatrixRotationQuaternion(rotation) * dx::XMMatrixTranslationFromVector(positionOffset));
}
