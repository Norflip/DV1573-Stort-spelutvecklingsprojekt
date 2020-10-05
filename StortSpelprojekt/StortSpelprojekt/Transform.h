#pragma once
#include <vector>
#include <DirectXMath.h>
namespace dx = DirectX;

class Object;

class Transform
{
public:
	Transform(Object* owner);
	Transform(Object* owner, dx::XMVECTOR position, dx::XMVECTOR rotation, dx::XMVECTOR scale);
	virtual ~Transform();

	dx::XMMATRIX GetWorldMatrix() const;
	dx::XMMATRIX GetLocalWorldMatrix() const;
	DirectX::XMVECTOR TransformDirection(DirectX::XMVECTOR direction) const;
	
	void Translate(float x, float y, float z);
	void Rotate(float pitch, float yaw, float roll);

	bool HasParent() const { return this->parent != nullptr; }
	void SetParent(Transform* parent) { this->parent = parent; changedThisFrame = true; }

	void AddChild(Transform* child);
	void RemoveChild(Transform* child);
	bool ContainsChild(Transform* child) const;

	size_t CountChildren() const { return this->children.size(); }
	std::vector<Transform*> GetChildren() const { return this->children; }

	static void SetParentChild(Transform& parent, Transform& child);
	static void RemoveParentChild(Transform& parent, Transform& child);

	Transform* GetParent() const { return this->parent; }
	Object* GetOwner() const { return this->owner; }

	bool ChangedThisFrame() const { return this->changedThisFrame; }
	void MarkNotChanged() { this->changedThisFrame = false; }

#pragma region SETTERS AND GETTERS
	dx::XMVECTOR GetWorldPosition() const;
	dx::XMVECTOR GetPosition() const { return dx::XMLoadFloat3(&this->position); }
	void SetPosition(dx::XMVECTOR position) { dx::XMStoreFloat3(&this->position, position); changedThisFrame = true; }

	dx::XMVECTOR GetScale() const { return dx::XMLoadFloat3(&this->scale); }
	void SetScale(dx::XMVECTOR scale) { dx::XMStoreFloat3(&this->scale, scale); changedThisFrame = true; }

	dx::XMVECTOR GetRotation() const { return dx::XMLoadFloat3(&this->rotation); }
	void SetRotation(dx::XMVECTOR rotation) { dx::XMStoreFloat3(&this->rotation, rotation); changedThisFrame = true; }
	

#pragma endregion

private:
	bool changedThisFrame;
	std::vector<Transform*> children;
	Transform* parent;
	Object* owner;

	dx::XMFLOAT3 position;
	dx::XMFLOAT3 rotation;
	dx::XMFLOAT3 scale;
};