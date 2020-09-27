#include "Transform.h"

Transform::Transform(Object* owner) : Transform(owner, dx::XMVectorZero(), dx::XMVectorZero(), { 1,1,1 })
{
	changedThisFrame = true;
}

Transform::Transform(Object* owner, dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale) : owner(owner), position(position), rotation(rotation), scale(scale), parent(nullptr)
{
	changedThisFrame = true;
}

Transform::~Transform()
{
}

void Transform::Translate(float x, float y, float z)
{
	if (x != 0.0f || y != 0.0f || z != 0.0f)
	{
		changedThisFrame = true;
		DirectX::XMFLOAT3 pos(x, y, z);
		this->position = dx::XMVectorAdd(this->position, dx::XMLoadFloat3(&pos));
	}
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	if (pitch != 0.0f || yaw != 0.0f || roll != 0.0f)
	{
		changedThisFrame = true;
		DirectX::XMFLOAT3 rot(pitch, yaw, roll);
		this->rotation = DirectX::XMVectorAdd(this->rotation, DirectX::XMLoadFloat3(&rot));

		//float p = std::fmaxf(-maxPitch, std::fminf(DirectX::XMVectorGetByIndex(this->rotation, 0), maxPitch));
		//this->rotation = DirectX::XMVectorSetByIndex(this->rotation, p, 0);
	}
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

void Transform::SetParentChild(Transform& parent, Transform& child)
{
	parent.AddChild(&child);
	child.SetParent(&parent);
}

void Transform::RemoveParentChild(Transform& parent, Transform& child)
{
	parent.RemoveChild(&child);
	child.SetParent(nullptr);
}

dx::XMMATRIX Transform::GetWorldMatrix() const
{
	dx::XMMATRIX worldMatrix = GetLocalWorldMatrix();
	if (parent != nullptr)
		worldMatrix = dx::XMMatrixMultiply(worldMatrix, parent->GetWorldMatrix());

	return worldMatrix;
}

dx::XMMATRIX Transform::GetLocalWorldMatrix() const
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
