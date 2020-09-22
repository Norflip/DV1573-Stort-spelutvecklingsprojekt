#pragma once
#include <vector>

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Renderer.h"
#include "Component.h"

class CameraComponent;

enum class ObjectFlag : unsigned int
{
	NONE = 0,
	ENABLED = 1 << 0,
	RENDER = 1 << 1,
	REMOVED = 1 << 2,
	DEFAULT = ENABLED | RENDER
};

DEFINE_ENUM_FLAG_OPERATORS(ObjectFlag)

class Object
{
public:
	Object(const std::string& name, ObjectFlag flag = ObjectFlag::DEFAULT);
	virtual ~Object();

	void Update(const float& deltaTime);
	//void FixedUpdate(const float& fixedDeltaTime);
	void Draw(Renderer* renderer, CameraComponent* camera);


	template <typename T>
	bool HasComponent() const;

	template <typename T, typename... TArgs> 
	T* AddComponent(TArgs&&... mArgs);

	template <typename T> T* 
	GetComponent() const;


	bool HasFlag(ObjectFlag flag) const;
	void AddFlag(ObjectFlag flag);
	void RemoveFlag(ObjectFlag flag);

	void SetName(const std::string& name) { this->name = name; }
	std::string GetName() const { return this->name; }

	Transform& GetTransform() { return this->transform; }

	

private:
	ObjectFlag flags;
	Transform transform;
	std::string name;

	std::vector<Component*> components;
	ComponentArray componentArray;
	ComponentBitSet componentBitSet;
};

template<typename T>
inline bool Object::HasComponent() const
{
	return componentBitSet[GetComponentTypeID<T>()];
}

template<typename T, typename ...TArgs>
inline T* Object::AddComponent(TArgs&& ...mArgs)
{
	T* c(new T(std::forward<TArgs>(mArgs)...));
	c->SetOwner(this);
	c->Initialize();

	components.push_back(c);
	componentArray[GetComponentTypeID<T>()] = c;
	componentBitSet[GetComponentTypeID<T>()] = true;

	return c;
}

template<typename T>
inline T* Object::GetComponent() const
{
	auto ptr(componentArray[GetComponentTypeID<T>()]);
	return static_cast<T*>(ptr);
}
