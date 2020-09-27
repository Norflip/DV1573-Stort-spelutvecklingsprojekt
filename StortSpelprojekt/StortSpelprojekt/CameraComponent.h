#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>
#include "Object.h"
#include "BoundingBoxes.h"

namespace dx = DirectX;
constexpr float CAMERA_NEAR_Z = 0.01f;
constexpr float CAMERA_FAR_Z = 400.0f;// 1000.0f; I changed this for the sake of culling


class CameraComponent : public Component
{
public:
	CameraComponent(float fieldOfView);
	virtual ~CameraComponent();

	void Resize(size_t width, size_t height);
	dx::XMMATRIX GetViewMatrix() const;
	dx::XMMATRIX GetProjectionMatrix() const { return this->projection; }
	const dx::XMMATRIX& GetVIewAndProjectionMatrix() { return GetViewMatrix()* GetProjectionMatrix(); }
	std::vector<dx::XMFLOAT4> GetFrustumPlanes();

	bool CullAgainstAABB(const AABB& aabb, const dx::XMFLOAT3 worldPos);

private:
	void UpdateProjectionMatrix();

private:
	dx::XMMATRIX projection;
	dx::XMMATRIX view;
	std::vector<dx::XMFLOAT4> frustumPlanes;

	size_t width, height;
	float fieldOfView;
};