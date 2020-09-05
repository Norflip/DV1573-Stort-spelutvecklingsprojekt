#pragma once

#include "Transform.h"
#include <DirectXMath.h>

namespace dx = DirectX;

class Camera
{
public:
	Camera(float fieldOfView, float aspectRatio);
	virtual ~Camera();

	Transform& GetTransform() { return this->transform; }

	dx::XMMATRIX GetViewMatrix() const;
	dx::XMMATRIX GetProjectionMatrix() const;

private:
	Transform transform;
	float fieldOfView;
	float aspectRatio;
};