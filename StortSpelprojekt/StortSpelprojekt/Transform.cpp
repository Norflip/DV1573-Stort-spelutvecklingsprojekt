#include "Transform.h"

Transform::Transform() : Transform(dx::XMVectorZero(), dx::XMVectorZero(), dx::XMVectorSplatOne())
{
}

Transform::Transform(dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale) : position(position), rotation(rotation), scale(scale), parent(nullptr)
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

void Transform::AddChild(Transform* child)
{
	if (child != nullptr)
	{
		this->children.push_back(child);
	}
}

void Transform::RemoveChild(Transform* child)
{
	for (auto i = children.begin(); i < children.end(); i++)
	{
		if ((*i) == child)
		{
			children.erase(i);
			break;
		}
	}
}

bool Transform::ContainsChild(Transform* child) const
{
	bool contains = false;
	for (auto i = children.cbegin(); i < children.cend() && !contains; i++)
	{
		contains = ((*i) == child);
	}
	return contains;
}

dx::XMMATRIX Transform::GetWorldMatrix() const
{
	dx::XMMATRIX worldMatrix = dx::XMMatrixScalingFromVector(this->scale) *
		dx::XMMatrixRotationRollPitchYawFromVector(this->rotation) *
		dx::XMMatrixTranslationFromVector(this->position);

	/*if (parent != nullptr)
		worldMatrix = dx::XMMatrixMultiply(parent->GetWorldMatrix(), worldMatrix);*/

	return worldMatrix;
}

DirectX::XMVECTOR Transform::TransformDirection(DirectX::XMVECTOR direction) const
{
	dx::XMMATRIX rot = dx::XMMatrixRotationRollPitchYawFromVector(this->rotation);		// rotation matrix
	return dx::XMVector3Normalize(DirectX::XMVector3TransformNormal(direction, rot));	// rotates the direction with the matrix
}
