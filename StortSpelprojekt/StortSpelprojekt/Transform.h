#pragma once
#include <vector>
#include <SimpleMath.h>
namespace dx = DirectX;

class Object;

#define ASSERT_STATIC_OBJECT assert(!GetOwner()->HasFlag(ObjectFlag::STATIC))

class Transform
{
public:
	Transform(Object* owner);
	Transform(Object* owner, dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale);
	virtual ~Transform();

	dx::XMMATRIX GetWorldMatrix() const;
	dx::XMMATRIX GetLocalWorldMatrix() const;
	DirectX::XMVECTOR TransformDirection(DirectX::XMVECTOR direction) const;
	DirectX::XMVECTOR TransformDirectionCustomRotation(DirectX::XMVECTOR direction, DirectX::XMVECTOR cRotation) const;

	void Translate(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);

	Object* GetOwner() const { return this->owner; }
	bool ChangedThisFrame() const { return this->changedThisFrame; }
	void SetChanged(bool changed) { this->changedThisFrame = changed; }

	dx::XMVECTOR GetPosition() const { return this->GetWorldPosition(); }
	dx::XMVECTOR GetLocalPosition();
	dx::XMVECTOR GetWorldPosition() const;
	void SetPosition(dx::XMVECTOR position) { SetWorldPosition(position); }
	void SetLocalPosition(dx::XMVECTOR position);
	void SetWorldPosition(dx::XMVECTOR position);

	dx::XMVECTOR GetRotation() const { return this->GetWorldRotation(); }
	dx::XMVECTOR GetLocalRotation() const;
	dx::XMVECTOR GetWorldRotation() const;
	void SetRotation(dx::XMVECTOR rotation) { SetWorldRotation(rotation); }
	void SetLocalRotation(dx::XMVECTOR rotation);
	void SetWorldRotation(dx::XMVECTOR rotation);
	
	dx::XMVECTOR GetScale() const { return dx::XMLoadFloat3(&this->scale); }
	void SetScale(dx::XMVECTOR scale);

	void SmoothRotation(dx::XMFLOAT3 endPos, float deltaTime, bool changeDir);

private:
	bool changedThisFrame;
	Object* owner;

	dx::XMFLOAT3 localPosition;
	dx::XMFLOAT4 localRotation;
	dx::XMFLOAT3 scale;
};