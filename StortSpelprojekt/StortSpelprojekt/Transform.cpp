#include "Transform.h"

Transform::Transform(Object* owner) : Transform(owner,dx::XMVectorZero(), dx::XMVectorZero(), dx::XMVectorSplatOne())
{
	changedThisFrame = true;
}

Transform::Transform(Object* owner, dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale) : owner(owner), parent(nullptr)
{
	dx::XMStoreFloat3(&this->position, position);
	dx::XMStoreFloat3(&this->rotation, rotation);
	dx::XMStoreFloat3(&this->scale, scale);
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

		position.x += x;
		position.y += y;
		position.z += z;
	}
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	if (pitch != 0.0f || yaw != 0.0f || roll != 0.0f)
	{
		changedThisFrame = true;

		rotation.x += pitch;
		rotation.y += yaw;
		rotation.z += roll;
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

dx::XMVECTOR Transform::GetWorldPosition() const
{
	dx::XMVECTOR pos, rot, scale;
	dx::XMMatrixDecompose(&scale, &rot, &pos, GetWorldMatrix());
	return pos;
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
	return dx::XMMatrixScalingFromVector(dx::XMLoadFloat3(&this->scale)) *
		dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat3(&this->rotation)) *
		dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&this->position));
}



DirectX::XMVECTOR Transform::TransformDirection(DirectX::XMVECTOR direction) const
{
	dx::XMMATRIX rot = dx::XMMatrixRotationRollPitchYawFromVector(dx::XMLoadFloat3(&this->rotation));		// rotation matrix
	return dx::XMVector3Normalize(DirectX::XMVector3TransformNormal(direction, rot));	// rotates the direction with the matrix
}
