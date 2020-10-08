#pragma once
#include "Math.h"
#include <DirectXMath.h>
namespace dx = DirectX;

struct Ray
{
	dx::XMFLOAT3 origin, direction;
	Ray() {}
	Ray(dx::XMFLOAT3 origin, dx::XMFLOAT3 direction) : origin(origin) 
	{
		dx::XMStoreFloat3(&this->direction, dx::XMVector3Normalize(dx::XMLoadFloat3(&direction)));
	}

	dx::XMFLOAT3 GetPoint(float distance) const
	{
		float x = origin.x + direction.x * distance;
		float y = origin.y + direction.y * distance;
		float z = origin.z + direction.z * distance;
		return dx::XMFLOAT3(x, y, z);
	}
};
