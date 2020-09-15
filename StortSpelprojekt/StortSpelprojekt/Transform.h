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
	
	void Rotate(float pitch, float yaw, float roll);

	bool HasParent() const { return this->parent != nullptr; }
	void SetParent(Transform* parent) { this->parent = parent; }

	void AddChild(Transform* child);
	void RemoveChild(Transform* child);
	bool ContainsChild(Transform* child) const;

	size_t CountChildren() const { return this->children.size(); }
	std::vector<Transform*> GetChildren() const { return this->children; }

	static void SkapaPäron(Transform& parent, Transform& child);

	Object* GetOwner() const { return this->owner; }

#pragma region SETTERS AND GETTERS
	dx::XMVECTOR GetPosition() const { return this->position; }
	void SetPosition(dx::XMVECTOR position) { this->position = position; }

	dx::XMVECTOR GetScale() const { return this->scale; }
	void SetScale(dx::XMVECTOR scale) { this->scale = scale; }

	dx::XMVECTOR GetRotation() const { return this->rotation; }
	void SetRotation(dx::XMVECTOR rotation) { this->rotation = rotation; }

#pragma endregion

private:
	std::vector<Transform*> children;
	Transform* parent;
	Object* owner;

	dx::XMVECTOR position;
	dx::XMVECTOR rotation;
	dx::XMVECTOR scale;
};