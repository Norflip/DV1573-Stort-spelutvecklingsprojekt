#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

__declspec(align(16))
struct cb_Object
{
	dx::XMMATRIX mvp;
	dx::XMMATRIX world;
};