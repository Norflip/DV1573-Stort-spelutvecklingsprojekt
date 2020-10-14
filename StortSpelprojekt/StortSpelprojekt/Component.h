#pragma once
#include <DirectXMath.h>

class Component;
class Renderer;
class CameraComponent;
class Object;

inline size_t GetComponentID()
{
	static size_t lastID = 0;
	return lastID++;
}

template <typename T>
inline size_t GetComponentTypeID() noexcept
{
	static size_t typeID = GetComponentID();
	return typeID;
}

class Component
{
public:
	Component() : owner(nullptr) {}
	virtual ~Component() {};

	virtual void Initialize() {};
	virtual void Update(const float& deltaTime) {};
	virtual void FixedUpdate(const float& fixedDeltaTime) {};
	virtual void Draw(Renderer* renderer, CameraComponent* camera) {};

	void SetOwner(Object* owner) { this->owner = owner; }
	Object* GetOwner() const { return this->owner; }

private:
	Object* owner;
};