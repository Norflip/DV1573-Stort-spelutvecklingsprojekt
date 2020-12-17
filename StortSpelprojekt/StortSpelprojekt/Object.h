#pragma once
#include <memory>
#include <algorithm>
#include "GrassComponent.h"
#include "MeshComponent.h"
#include "SkeletonMeshComponent.h"
//#include "Transform.h"
//#include "Mesh.h"
//#include "Material.h"
//#include "Renderer.h"
//#include "Component.h"

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


enum class MessageType
{
	SET_FLAG,
	REM_FLAG,
	ADD_CHILD,
	REM_CHILD,
	PARENT_CHANGED,
};

// max components on entity
constexpr std::size_t maxComponents = 40;
using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<std::vector<Component*>, maxComponents>;

ALIGN16
class Object
{
	static size_t idCounter;

public:
	Object(const std::string& name, ObjectFlag flag = ObjectFlag::DEFAULT);
	virtual ~Object();

	void Update(const float& deltaTime);
	void FixedUpdate(const float& fixedDeltaTime);
	void Draw(Renderer* renderer, CameraComponent* camera);
	void Reset();

	template <typename T>
	bool HasComponent() const;

	template <typename T, typename... TArgs>
	T* AddComponent(TArgs&&... mArgs);

	template <typename T>
	T* GetComponent() const;

	template <typename T>
	std::vector<T*> GetComponents() const;
	
	template <typename T>
	std::vector<T*> GetComponentsOfSubType() const;

	bool HasFlag(ObjectFlag flag) const;
	void AddFlag(ObjectFlag flag, bool sendMsg = true);
	void RemoveFlag(ObjectFlag flag, bool sendMsg = true);

	void SetName(const std::string& name) { this->name = name; }
	std::string GetName() const { return this->name; }

	Transform& GetTransform() { return this->transform; }

	size_t GetID() const { return this->id; }
	ALIGN16_ALLOC;

	bool HasParent() const { return this->parent != nullptr; }
	void SetParent(Object* parent);

	void AddChild(Object* child);
	bool RemoveChild(Object* child);
	bool ContainsChild(Object* child) const;

	size_t CountChildren() const { return this->children.size(); }
	std::vector<Object*> GetChildren() const { return this->children; }
	Object* GetChild(size_t index) const { assert(index >= 0 && index < children.size()); return this->children[index]; }

	static void AddToHierarchy(Object* root, Object* obj);
	static void RemoveFromHierarchy(Object* obj);

	Object* GetParent() const { return this->parent; }

	void SendMsg(const int& type, const std::string& msg, Object* sender, bool sendDown = true, void* data = nullptr);
	void SetEnable(bool state) { this->enable = state; }

private:
	ObjectFlag flags;
	Transform transform;
	std::string name;
	size_t id;

	Object* parent;
	std::vector<Object*> children;

	std::vector<Component*> components;
	ComponentArray componentArray;
	ComponentBitSet componentBitSet;

	bool enable;
	friend Component;
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

	assert(GetComponentTypeID<T>() < maxComponents);
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
inline std::vector<T*> Object::GetComponentsOfSubType() const
{
	std::vector<T*> items;

	for (size_t i = 0; i < maxComponents; i++)
	{
		size_t size = componentArray[i].size();

		if (componentBitSet[i] && size > 0)
		{
			T* t = dynamic_cast<T*>(*componentArray[i].begin());

			if (t != nullptr)	// valid 
			{
				items.push_back(t);
				for (size_t j = 1; j < size; j++)
				{
					items.push_back(dynamic_cast<T*>(componentArray[i][j]));
				}
			}
		}
	}

	return items;
}


