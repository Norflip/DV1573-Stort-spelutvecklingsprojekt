#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

constexpr size_t CB_OBJECT_SLOT = 0;
constexpr size_t CB_MATERIAL_SLOT = 1;
constexpr size_t CB_SCENE_SLOT = 2;
constexpr size_t CB_LIGHT_SLOT = 3;

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
	dx::XMFLOAT3 albedo;
};

__declspec(align(16))
struct cb_Scene
{

};

//struct cb_PointLight
//{
//	dx::XMFLOAT4 plAmbient;
//	dx::XMFLOAT4 plDiffuse;
//	dx::XMFLOAT3 plPosition;
//	float plRange;
//	dx::XMFLOAT3 att;
//	float padding;
//};
//
//struct cb_DirectionalLight
//{
//	dx::XMFLOAT4 Direction;
//	dx::XMFLOAT4 Color;
//};

struct cb_Lights
{
	dx::XMFLOAT4 Direction;
	dx::XMFLOAT4 Color;
	dx::XMFLOAT3 Position;
	float type;
	dx::XMFLOAT3 attenuation;
	float intensity;
};