#include "Camera.h"

Camera::Camera(float fieldOfView, float aspectRatio) : fieldOfView(fieldOfView), aspectRatio(aspectRatio)
{
}

Camera::~Camera()
{
}

dx::XMMATRIX Camera::GetViewMatrix() const
{
	return dx::XMMatrixLookToLH(
		transform.GetPosition(),
		transform.TransformDirection({ 0,0,1 }),
		transform.TransformDirection({ 0,1,0 })
	);
}

dx::XMMATRIX Camera::GetProjectionMatrix() const
{
	const float toRadians = 0.0174532925f;
	return DirectX::XMMatrixPerspectiveFovLH(fieldOfView * toRadians, aspectRatio, NEAR_Z, FAR_Z);
}
