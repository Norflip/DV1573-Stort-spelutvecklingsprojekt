#include "LightManager.h"
#include "PointLightComponent.h"

LightManager::LightManager()
{
}

LightManager::~LightManager()
{
}

void LightManager::Initialize(ID3D11Device* device)
{
	DXHelper::CreateConstBuffer(device, &light_cbuffer, &cb_scene, sizeof(cb_scene));

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

void LightManager::UpdateBuffers(ID3D11DeviceContext* context, dx::XMVECTOR camPos)
{
	cb_scene.sunDirection = dx::XMFLOAT3(0.0f, 100.0f, -45.0f);
	cb_scene.sunIntensity = 0.4f;

	for (auto i = pointLightMap.begin(); i != pointLightMap.end(); i++)
	{
		cb_scene.pointLights[i->first].lightColor = i->second->GetColor();
		dx::XMStoreFloat3(&cb_scene.pointLights[i->first].lightPosition, (i->second->GetOwner()->GetTransform().GetPosition()));
		cb_scene.pointLights[i->first].attenuation = i->second->GetAttenuation();
		cb_scene.pointLights[i->first].range = i->second->GetRange();
	}
	cb_scene.nrOfPointLights = pointLightMap.size();

	// Temporärt för att ändra skybox texture
	static int ids = 0;
	static float color = 0.0f;

	color += (float)0.005f;
	if (color > 1.0f)
	{
		color -= 1.0f;
		if (ids != 3)
			ids += 1;
		else
			ids = 0;
	}

	cb_scene.id = ids;
	cb_scene.factor = color;


	std::cout << "CamPos: " << camPos.m128_f32[0] << " " << camPos.m128_f32[1] << " " << camPos.m128_f32[2] << std::endl;

	dx::XMStoreFloat3(&cb_scene.cameraPosition, camPos);
	DXHelper::BindConstBuffer(context, light_cbuffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::DOMAINS);
	DXHelper::BindConstBuffer(context, light_cbuffer, &cb_scene, CB_SCENE_SLOT, ShaderBindFlag::PIXEL);
}
