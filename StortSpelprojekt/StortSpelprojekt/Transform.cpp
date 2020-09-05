#include "Transform.h"

Transform::Transform() : position(dx::XMVectorZero()), rotation(dx::XMVectorZero()), scale(dx::XMVectorSplatOne())
{
}

Transform::Transform(dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale) : position(position), rotation(rotation), scale(scale)
{
}

Transform::~Transform()
{
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	DirectX::XMFLOAT3 rot(pitch, yaw, roll);
	this->rotation = DirectX::XMVectorAdd(this->rotation, DirectX::XMLoadFloat3(&rot));

	//float p = std::fmaxf(-maxPitch, std::fminf(DirectX::XMVectorGetByIndex(this->rotation, 0), maxPitch));
	//this->rotation = DirectX::XMVectorSetByIndex(this->rotation, p, 0);
}

dx::XMMATRIX Transform::GetWorldMatrix() const
{
	return dx::XMMatrixScalingFromVector(this->scale) * 
		dx::XMMatrixRotationRollPitchYawFromVector(this->rotation) * 
		dx::XMMatrixTranslationFromVector(this->position);
}

DirectX::XMVECTOR Transform::TransformDirection(DirectX::XMVECTOR direction) const
{
	dx::XMMATRIX rot = dx::XMMatrixRotationRollPitchYawFromVector(this->rotation);		// rotation matrix
	return dx::XMVector3Normalize(DirectX::XMVector3TransformNormal(direction, rot));	// rotates the direction with the matrix
}
