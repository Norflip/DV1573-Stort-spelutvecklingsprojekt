#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

constexpr size_t CB_OBJECT_SLOT = 0;
constexpr size_t CB_MATERIAL_SLOT = 1;
constexpr size_t CB_SCENE_SLOT = 2;
constexpr size_t CB_SKELETON_SLOT = 3;
constexpr size_t CB_LIGHT_SLOT = 4;

enum LightType { POINTLIGHT, DIRLIGHT };


__declspec(align(16))
struct cb_Object
{
	dx::XMFLOAT4X4 mvp;
	dx::XMFLOAT4X4 world;
};

__declspec(align(16))
struct cb_Material
{
	dx::XMFLOAT3 ambient;
	float pad1;
	dx::XMFLOAT3 albedo;
	float pad2;
	dx::XMFLOAT3 specular;
	float specularFactor;
};

__declspec(align(16))
struct cb_Scene
{

};

__declspec(align(16))
struct cb_Skeleton
{
	dx::XMFLOAT4X4 bones[60]; //Can you use a structured buffer instead?
};
struct cb_Lights
{
	dx::XMFLOAT4 Direction;
	dx::XMFLOAT4 Color;
	dx::XMFLOAT3 Position;
	float type;
	dx::XMFLOAT3 attenuation;
	float intensity;
};