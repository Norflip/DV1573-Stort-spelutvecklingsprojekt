#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

constexpr size_t CB_OBJECT_SLOT = 0;
constexpr size_t CB_MATERIAL_SLOT = 1;
constexpr size_t CB_SCENE_SLOT = 2; 
constexpr size_t CB_LIGHT_SLOT = 3;
constexpr size_t CB_GRASS_PARAMETERS_SLOT = 4;

constexpr size_t POINT_LIGHT_COUNT = 32;

ALIGN16
struct cb_Object
{
	dx::XMFLOAT4X4 mvp;
	dx::XMFLOAT4X4 world;
	dx::XMFLOAT4X4 vp;
	dx::XMFLOAT4X4 wv;
};

ALIGN16
struct cb_Material
{
	dx::XMFLOAT4 ambient;
	dx::XMFLOAT4 diffuse;
	dx::XMFLOAT4 specular;
	
	int hasAlbedo;
	int hasNormalMap;
	float paddingOne;
	float paddingTwo;
};

ALIGN16
struct s_PointLight
{
	dx::XMFLOAT4 lightColor;
	dx::XMFLOAT3 lightPosition;
	float range;
	dx::XMFLOAT3 attenuation;
	float paddingThree;
};

ALIGN16
struct cb_Lights
{
	s_PointLight pointLights[POINT_LIGHT_COUNT];
	int nrOfPointLights;
	dx::XMFLOAT3 sunDirection;
	float sunIntensity;
	dx::XMFLOAT3 pad;
};

ALIGN16
struct cb_Scene
{
	dx::XMFLOAT4X4 invProjection;

	dx::XMFLOAT4X4 invView;

	dx::XMFLOAT3 cameraPosition;
	int id;

	float factor;
	float time;
	dx::XMFLOAT2 mousePos;

	dx::XMFLOAT2 screenSize;
	float distanceToHouse;
	float pad_cbs;

};

ALIGN16
struct cb_grass
{
	float pixelSize;
	float grassWidth;
	float grassRadius;
	float grassDisplacement;
};




