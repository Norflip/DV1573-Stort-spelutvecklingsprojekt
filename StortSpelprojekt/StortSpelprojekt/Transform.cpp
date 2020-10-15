#include "Transform.h"
#include "Object.h"

Transform::Transform(Object* owner) : Transform(owner,dx::XMVectorZero(), dx::XMQuaternionIdentity(), dx::XMVectorSplatOne())
{
	changedThisFrame = true;
}

Transform::Transform(Object* owner, dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale) : owner(owner), parent(nullptr)
{
	dx::XMStoreFloat3(&this->position, position);
	dx::XMStoreFloat4(&this->rotation, rotation);
	dx::XMStoreFloat3(&this->scale, scale);
	changedThisFrame = true;
}

Transform::~Transform()
{
}

void Transform::Translate(float x, float y, float z)
{
	ASSERT_STATIC_OBJECT;

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
	ASSERT_STATIC_OBJECT;

	if (pitch != 0.0f || yaw != 0.0f || roll != 0.0f)
	{
		changedThisFrame = true;

		dx::XMVECTOR right = TransformDirection({ 1,0,0 });
		dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, pitch), dx::XMQuaternionRotationAxis({ 0,1,0 }, yaw));


		//dx::XMVECTOR eulerRotation = dx::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll);
		dx::XMVECTOR newRotation = dx::XMQuaternionMultiply(dx::XMLoadFloat4(&rotation), eulerRotation);

		dx::XMStoreFloat4(&rotation, newRotation);

		//rotationQuaternion.x += pitch;
		//rotationQuaternion.y += yaw;
		//rotationQuaternion.z += roll;
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

void Transform::SetPosition(dx::XMVECTOR position)
{
	ASSERT_STATIC_OBJECT;
	dx::XMStoreFloat3(&this->position, position); 
	changedThisFrame = true;
}

void Transform::SetScale(dx::XMVECTOR scale)
{
	ASSERT_STATIC_OBJECT;
	dx::XMStoreFloat3(&this->scale, scale); 
	changedThisFrame = true;
}

void Transform::SetRotation(dx::XMVECTOR rotation)
{
	ASSERT_STATIC_OBJECT;
	dx::XMStoreFloat4(&this->rotation, rotation); 
	changedThisFrame = true;
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
		dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&this->rotation)) *
		dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&this->position));
}



DirectX::XMVECTOR Transform::TransformDirection(DirectX::XMVECTOR direction) const
{

	//dx::XMMATRIX rot = dx::XMMatrixRotationQuaternion();		// rotation matrix
	return DirectX::XMVector3Rotate(dx::XMVector3Normalize(direction), dx::XMLoadFloat4(&this->rotation));	// rotates the direction with the matrix
}

//DirectX::XMVECTOR Transform::TransformDirectionCustomRotation(DirectX::XMVECTOR direction, DirectX::XMVECTOR cRotation) const
//{
//
//	//dx::XMMATRIX rot = dx::XMMatrixRotationQuaternion();		// rotation matrix
//	return DirectX::XMVector3Rotate(dx::XMVector3Normalize(direction), cRotation);	// rotates the direction with the matrix
//}
