#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

constexpr size_t CB_OBJECT_SLOT = 0;
constexpr size_t CB_MATERIAL_SLOT = 1;
constexpr size_t CB_SCENE_SLOT = 2;

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

struct cb_light
{

};