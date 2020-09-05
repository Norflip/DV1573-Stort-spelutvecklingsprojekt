#pragma once
#include <DirectXMath.h>
namespace dx = DirectX;

class Transform
{
public:
	Transform();
	Transform(dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale);
	virtual ~Transform();

	dx::XMMATRIX GetWorldMatrix() const;
	DirectX::XMVECTOR TransformDirection(DirectX::XMVECTOR direction) const;

	void Rotate(float pitch, float yaw, float roll);

public:
	// SETTERS AND GETTERS
	dx::XMVECTOR position;
	dx::XMVECTOR rotation;
	dx::XMVECTOR scale;
};