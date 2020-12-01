#pragma once
#include <DirectXMath.h>

class Component;
class Renderer;
class CameraComponent;
class Object;
enum class ObjectFlag : unsigned int;

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

ALIGN16
class Component
{
public:
	Component() : owner(nullptr), enable(true) {}
	virtual ~Component() {};

	virtual void Initialize() {};
	virtual void Update(const float& deltaTime) {};
	virtual void FixedUpdate(const float& fixedDeltaTime) {};
	virtual void Draw(Renderer* renderer, CameraComponent* camera) {};
	virtual void Reset() {};

	virtual void OnOwnerFlagChanged(ObjectFlag oldFlag, ObjectFlag newFlag) {}
	void SetOwner(Object* owner) { this->owner = owner; }
	Object* GetOwner() const { return this->owner; }

	bool IsEnabled() const {return enable;}
	void SetEnabled(bool enable) { this->enable = enable; }
	ALIGN16_ALLOC;

private:
	virtual void RecieveMsg(const int& type, const std::string& msg, Object* sender, void* data = nullptr) {}

private:
	Object* owner;
	bool enable;

	friend Object;
};