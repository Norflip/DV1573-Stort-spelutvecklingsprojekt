#pragma once
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

class Component;
class Renderer;
class CameraComponent;
class Object;

// max components on entity
constexpr std::size_t maxComponents = 16;
using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<Component*, maxComponents>;

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