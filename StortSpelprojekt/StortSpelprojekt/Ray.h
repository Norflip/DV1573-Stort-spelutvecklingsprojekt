#pragma once
#include "Math.h"
namespace dx = DirectX;

struct Ray
{
	dx::XMFLOAT3 origin, direction;
	Ray(dx::XMFLOAT3 origin, dx::XMFLOAT3 direction) : origin(origin), direction(direction) {}

	dx::XMFLOAT3 GetPoint(float distance) const
	{
		float x = origin.x + direction.x * distance;
		float y = origin.y + direction.y * distance;
		float z = origin.z + direction.z * distance;
		return dx::XMFLOAT3(x, y, z);
	}
};

struct RayHit
{
	dx::XMFLOAT3 position;
	dx::XMFLOAT3 normal;

//	RigidBodyComp* body;
	Object* object;

	bool didHit;
};

