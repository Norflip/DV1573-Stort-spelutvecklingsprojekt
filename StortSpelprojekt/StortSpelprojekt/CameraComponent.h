#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>
#include "Object.h"
#include "BoundingBoxes.h"
#include "Ray.h"
#include "VirtualCamera.h"

namespace dx = DirectX;

constexpr float CAMERA_NEAR_Z = 0.01f;
constexpr float CAMERA_FAR_Z = 400.0f;// 1000.0f; I changed this for the sake of culling


class CameraComponent : public Component, public VirtualCamera
{
public:
	CameraComponent(size_t width, size_t height, float fieldOfView);
	virtual ~CameraComponent();

	void SetFOV(float fov);
	dx::XMMATRIX GetViewMatrix() const;

	void UpdateView();
	bool InView(const BoundingBox& bounds, const dx::XMMATRIX world);
	Ray MouseToRay(const unsigned int& x, const unsigned int& y) const;


private:
	std::vector<dx::XMFLOAT4> planes;
};

