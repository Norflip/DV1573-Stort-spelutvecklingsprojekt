#include "stdafx.h"
#include "CameraComponent.h"

CameraComponent::CameraComponent(size_t width, size_t height, float fieldOfView)
	:VirtualCamera(VirtualCamera::CameraMode::PERSPECTIVE, width, height, CAMERA_NEAR_Z, CAMERA_FAR_Z, fieldOfView)
{

}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::SetFOV(float fov)
{
	UpdateProjectionMatrix(width, height, fov, CAMERA_NEAR_Z, CAMERA_FAR_Z);
}

dx::XMMATRIX CameraComponent::GetViewMatrix() const
{
	return VirtualCamera::GetViewMatrix(GetOwner()->GetTransform().GetWorldMatrix());
}

void CameraComponent::UpdateView()
{
	dx::XMMATRIX transform = GetOwner()->GetTransform().GetWorldMatrix();
	planes = VirtualCamera::GetFrustumPlanes(transform);
}

bool CameraComponent::InView(const BoundingBox& bounds, const dx::XMMATRIX world)
{
	return VirtualCamera::InView(bounds, world, planes);
}

Ray CameraComponent::MouseToRay(const unsigned int& x, const unsigned int& y) const
{
	dx::XMVECTOR screenPosition = dx::XMVectorSet(FCAST(x), FCAST(y), 0.0f, 0.0f);
	dx::XMVECTOR worldPosition = dx::XMVector3Unproject(screenPosition, 0, 0, (float)width, (float)height, 0.0f, 1.0f, GetProjectionMatrix(), GetViewMatrix(), dx::XMMatrixIdentity());

	dx::XMVECTOR position = GetOwner()->GetTransform().GetPosition();
	dx::XMVECTOR dir = dx::XMVectorSubtract(worldPosition, position);

	dx::XMFLOAT3 direction, origin;
	dx::XMStoreFloat3(&direction, dx::XMVector3Normalize(dir));
	dx::XMStoreFloat3(&origin, position);
	return Ray(origin, direction);
}