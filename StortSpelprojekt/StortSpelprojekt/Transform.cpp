#include "Transform.h"
#include "Object.h"

Transform::Transform(Object* owner) : Transform(owner,dx::XMVectorZero(), dx::XMQuaternionIdentity(), dx::XMVectorSplatOne())
{
	changedThisFrame = true;
}

Transform::Transform(Object* owner, dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale) : owner(owner), parent(nullptr)
{
	dx::XMStoreFloat3(&this->localPosition, position);
	dx::XMStoreFloat4(&this->localRotation, rotation);
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

		localPosition.x += x;
		localPosition.y += y;
		localPosition.z += z;
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
		dx::XMVECTOR newRotation = dx::XMQuaternionMultiply(dx::XMLoadFloat4(&localRotation), eulerRotation);
		dx::XMStoreFloat4(&localRotation, newRotation);

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

void Transform::ClearFromHierarchy(Transform& transform)
{
	if (transform.parent != nullptr && transform.ContainsChild(&transform))
	{
		transform.parent->RemoveChild(&transform);
		transform.parent = nullptr;
	}
}




dx::XMVECTOR Transform::GetLocalPosition()
{
	return dx::XMLoadFloat3(&this->localPosition);
}

dx::XMVECTOR Transform::GetWorldPosition() const
{
	dx::XMVECTOR pos, rot, scale;
	dx::XMMatrixDecompose(&scale, &rot, &pos, GetWorldMatrix());
	return pos;
}

void Transform::SetLocalPosition(dx::XMVECTOR position)
{
	ASSERT_STATIC_OBJECT;
	dx::XMStoreFloat3(&this->localPosition, position);
	changedThisFrame = true;
}

void Transform::SetWorldPosition(dx::XMVECTOR position)
{
	dx::XMVECTOR pos = position;

	if (parent != nullptr)
	{
		dx::XMMATRIX invWorld = dx::XMMatrixInverse(nullptr, parent->GetWorldMatrix());
		pos = dx::XMVector3Transform(pos, invWorld);
	}

	SetLocalPosition(pos);
}


dx::XMVECTOR Transform::GetLocalRotation() const
{
	return dx::XMLoadFloat4(&this->localRotation);
}

dx::XMVECTOR Transform::GetWorldRotation() const
{
	dx::XMVECTOR pos, rot, scale;
	dx::XMMatrixDecompose(&scale, &rot, &pos, GetWorldMatrix());
	return rot;
}

void Transform::SetLocalRotation(dx::XMVECTOR rotation)
{
	ASSERT_STATIC_OBJECT;
	dx::XMStoreFloat4(&this->localRotation, rotation);
	changedThisFrame = true;
}

void Transform::SetWorldRotation(dx::XMVECTOR rotation)
{
	dx::XMVECTOR rot = rotation;

	if (parent != nullptr)
	{
		dx::XMMATRIX invWorld = dx::XMMatrixInverse(nullptr, parent->GetWorldMatrix());
		dx::XMVECTOR parentRot = dx::XMQuaternionInverse(parent->GetWorldRotation());
		rot = dx::XMQuaternionMultiply(rot, parentRot);
	}

	SetLocalRotation(rot);
}

void Transform::SetScale(dx::XMVECTOR scale)
{
	ASSERT_STATIC_OBJECT;
	dx::XMStoreFloat3(&this->scale, scale);
	changedThisFrame = true;
}

void Transform::SmoothRotation(dx::XMFLOAT3 endPos, float deltaTime, bool changeDir)
{
	//float nextDir = 0;
	//float currentDir = localRotation.y;
	//DirectX::XMVECTOR directionVector = { position.x - endPos.x,0, position.z - endPos.z };

	//if (changeDir)
	//	nextDir = atan2(DirectX::XMVectorGetByIndex(directionVector, 0), DirectX::XMVectorGetByIndex(directionVector, 2));

	//Rotate(0, Math::ShortestRotation(currentDir, nextDir) * (deltaTime * 3.14f), 0);

	//if (DirectX::XMVectorGetByIndex(GetRotation(), 1) < -Math::PI * 2)
	//	SetRotation({ 0, DirectX::XMVectorGetByIndex(GetRotation(), 1) + Math::PI * 2, 0 });
	//if (DirectX::XMVectorGetByIndex(GetRotation(), 1) > Math::PI * 2)
	//	SetRotation({ 0, DirectX::XMVectorGetByIndex(GetRotation(), 1) - Math::PI * 2, 0 });
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
		dx::XMMatrixRotationQuaternion(dx::XMLoadFloat4(&this->localRotation)) *
		dx::XMMatrixTranslationFromVector(dx::XMLoadFloat3(&this->localPosition));
}

DirectX::XMVECTOR Transform::TransformDirection(DirectX::XMVECTOR direction) const
{

	//dx::XMMATRIX rot = dx::XMMatrixRotationQuaternion();		// rotation matrix
	return DirectX::XMVector3Rotate(dx::XMVector3Normalize(direction), dx::XMLoadFloat4(&this->localRotation));	// rotates the direction with the matrix
}

DirectX::XMVECTOR Transform::TransformDirectionCustomRotation(DirectX::XMVECTOR direction, DirectX::XMVECTOR cRotation) const
{
	return DirectX::XMVector3Rotate(dx::XMVector3Normalize(direction), cRotation);	// rotates the direction with custom 
}
