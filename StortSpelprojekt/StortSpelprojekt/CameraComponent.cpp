#include "CameraComponent.h"

CameraComponent::CameraComponent(float fieldOfView) : fieldOfView(fieldOfView) {}
CameraComponent::~CameraComponent() {}

void CameraComponent::Resize(size_t width, size_t height)
{
	this->width = width;
	this->height = height;
	UpdateProjectionMatrix();
}

dx::XMMATRIX CameraComponent::GetViewMatrix() const
{
	Transform& transform = GetOwner()->GetTransform();

	return dx::XMMatrixLookToLH(
		transform.GetPosition(),
		transform.TransformDirection({ 0,0,1 }),
		transform.TransformDirection({ 0,1,0 })
	);
}

void CameraComponent::UpdateProjectionMatrix()
{
	const float toRadians = 0.0174532925f;
	float aspect = static_cast<float>(width) / static_cast<float>(height);
	this->projection = DirectX::XMMatrixPerspectiveFovLH(fieldOfView * toRadians, aspect, CAMERA_NEAR_Z, CAMERA_FAR_Z);
}
