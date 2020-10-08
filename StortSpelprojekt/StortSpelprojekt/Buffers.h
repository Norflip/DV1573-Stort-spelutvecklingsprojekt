#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

constexpr size_t CB_OBJECT_SLOT = 0;
constexpr size_t CB_MATERIAL_SLOT = 1;
constexpr size_t CB_SCENE_SLOT = 2;
constexpr size_t CB_GRASS_PARAMETERS_SLOT = 3;

constexpr size_t POINT_LIGHT_COUNT = 16;

__declspec(align(16))
struct cb_Object
{
	dx::XMFLOAT4X4 mvp;
	dx::XMFLOAT4X4 world;
	dx::XMFLOAT4X4 vp;
	dx::XMFLOAT4X4 wv;
};

__declspec(align(16))
struct cb_Material
{
	dx::XMFLOAT4 ambient;
	dx::XMFLOAT4 diffuse;
	dx::XMFLOAT4 specular;
	
	int hasAlbedo;
	int hasNormalMap;
};

__declspec(align(16))
struct s_PointLight
{
	dx::XMFLOAT4 lightColor;
	dx::XMFLOAT3 lightPosition;
	float range;
	dx::XMFLOAT3 attenuation;
};

struct cb_Lights
{
	s_PointLight pointLights[POINT_LIGHT_COUNT];
	int nrOfPointLights;
	dx::XMFLOAT3 sunDirection;
	float sunIntensity;
	dx::XMFLOAT3 cameraPosition;

	// new stuff
	int id;
	float factor;
	dx::XMFLOAT2 padding10;
	
	float time;
	dx::XMFLOAT3 padding11;
};


__declspec(align(16))
struct cb_Scene
{
	float time;
};

__declspec(align(16))

struct cb_grass
{
	float pixelSize;
	dx::XMFLOAT3 padding;
};	