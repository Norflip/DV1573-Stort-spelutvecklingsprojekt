#pragma once

#include "Transform.h"
#include <DirectXMath.h>

namespace dx = DirectX;
constexpr float NEAR_Z = 0.001f;
constexpr float FAR_Z = 1000.0f;

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