#pragma once
#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>

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
	NO_CULL = 1 << 3,
	STATIC = 1 << 4,
	DEFAULT = ENABLED | RENDER
};

DEFINE_ENUM_FLAG_OPERATORS(ObjectFlag)

// max components on entity
constexpr std::size_t maxComponents = 16;
using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<std::vector<Component*>, maxComponents>;

class Object
{
	static size_t idCounter;

public:
	Object(const std::string& name, ObjectFlag flag = ObjectFlag::DEFAULT);
	virtual ~Object();

	void Update(const float& deltaTime);
	void FixedUpdate(const float& fixedDeltaTime);
	void Draw(Renderer* renderer, CameraComponent* camera);

	template <typename T>
	bool HasComponent() const;

	template <typename T, typename... TArgs>
	T* AddComponent(TArgs&&... mArgs);

	template <typename T>
	T* GetComponent() const;

	template <typename T>
	std::vector<T*> GetComponents() const;

	template <typename T>
	T* GetComponentUpwards() const;

	template <typename T>
	std::vector<T*> GetComponentsUpwards() const;

	bool HasFlag(ObjectFlag flag) const;
	void AddFlag(ObjectFlag flag);
	void RemoveFlag(ObjectFlag flag);

	void SetName(const std::string& name) { this->name = name; }
	std::string GetName() const { return this->name; }

	Transform& GetTransform() { return this->transform; }

	size_t GetID() const { return this->id; }

private:
	ObjectFlag flags;
	Transform transform;
	std::string name;
	size_t id;

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
	componentArray[GetComponentTypeID<T>()].push_back(c);// = c;
	componentBitSet[GetComponentTypeID<T>()] = true;

	return c;
}

template<typename T>
inline T* Object::GetComponent() const
{
	auto comps = GetComponents<T>();
	return comps.size() > 0 ? comps[0] : nullptr;
}

template<typename T>
inline std::vector<T*> Object::GetComponents() const
{
	std::vector<T*> items;
	if (HasComponent<T>())
	{
		auto ptr(componentArray[GetComponentTypeID<T>()]);

		for (auto i = ptr.cbegin(); i < ptr.cend(); i++)
			items.push_back(static_cast<T*>(*i));
	}
	return items;
}

template<typename T>
inline T* Object::GetComponentUpwards() const
{
	std::vector<T*> components = GetComponentsUpwards();
	return components.size() > 0 ? components[0] : nullptr;
}

template<typename T>
inline std::vector<T*> Object::GetComponentsUpwards() const
{
	std::vector<T*> components;

	if (HasComponent<T>())
	{
		components = GetComponents<T>();
	}
	else
	{
		Transform* parent = transform.GetParent();
		if (parent != nullptr)
			components = transform.GetParent()->GetOwner()->GetComponentsUpwards();
	}

	return components;
}


