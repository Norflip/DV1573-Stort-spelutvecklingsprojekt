#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>
#include "Object.h"

namespace dx = DirectX;
constexpr float CAMERA_NEAR_Z = 0.01f;
constexpr float CAMERA_FAR_Z = 1000.0f;

class CameraComponent : public Component
{
public:
	CameraComponent(float fieldOfView);
	virtual ~CameraComponent();

	void Resize(size_t width, size_t height);
	dx::XMMATRIX GetViewMatrix() const;
	dx::XMMATRIX GetProjectionMatrix() const { return this->projection; }

private:
	void UpdateProjectionMatrix();

private:
	dx::XMMATRIX projection;
	dx::XMMATRIX view;

	size_t width, height;
	float fieldOfView;
};