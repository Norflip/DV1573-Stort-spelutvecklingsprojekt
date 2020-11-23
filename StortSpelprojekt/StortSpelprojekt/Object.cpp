#include "stdafx.h"
#include "Object.h"
#include "Component.h"

size_t Object::idCounter = 0;


Object::Object(const std::string& name, ObjectFlag flag) : name(name), flags(flag), transform(this), id(idCounter++), parent(nullptr)
{
	assert(!name.empty());
}

Object::~Object()
{
	for (auto i = components.begin(); i < components.end(); i++)
		delete (*i);
	components.clear();

	size_t childCount = children.size();
	if (childCount > 0)
	{
		for (size_t i = 0; i < childCount; i++)
		{
			delete children[i];
			children[i] = nullptr;
		}
		
		children.clear();
	}
}

void Object::Update(const float& deltaTime)
{
	if (HasFlag(ObjectFlag::ENABLED))
	{
		for (auto i = components.begin(); i < components.end(); i++)
		{
			if ((*i)->IsEnabled())
				(*i)->Update(deltaTime);
		}

		for (auto i = children.begin(); i < children.end(); i++)
			(*i)->Update(deltaTime);
	}
}

void Object::FixedUpdate(const float& fixedDeltaTime)
{
	if (HasFlag(ObjectFlag::ENABLED))
	{
		for (auto i = components.begin(); i < components.end(); i++)
			(*i)->FixedUpdate(fixedDeltaTime);

		for (auto i = children.begin(); i < children.end(); i++)
			(*i)->FixedUpdate(fixedDeltaTime);
	}
}

void Object::Draw(Renderer* renderer, CameraComponent* camera)
{
	if (HasFlag(ObjectFlag::ENABLED))
	{
		for (auto i = components.begin(); i < components.end(); i++)
		{
			if (HasFlag(ObjectFlag::RENDER))
				(*i)->Draw(renderer, camera);
		}

		for (auto i = children.begin(); i < children.end(); i++)
			(*i)->Draw(renderer, camera);
	}
}

void Object::Reset()
{
	for (auto i = components.begin(); i < components.end(); i++)
		(*i)->Reset();

	for (auto i = children.begin(); i < children.end(); i++)
		(*i)->Reset();
}

bool Object::HasFlag(ObjectFlag flag) const
{
	return (int)(flag & flags) != 0;
}

void Object::AddFlag(ObjectFlag flag)
{
	ObjectFlag old = flags;
	flags |= flag;

	//for (auto i = components.begin(); i < components.end(); i++)
	//	(*i)->OnOwnerFlagChanged(old, flags);
}

void Object::RemoveFlag(ObjectFlag flag)
{
	ObjectFlag old = this->flags;
	this->flags &= ~flag;


	//for (auto i = components.begin(); i < components.end(); i++)
		//(*i)->OnOwnerFlagChanged(old, this->flags);
}

void Object::AddChild(Object* child)
{
	assert(child != nullptr);
	this->children.push_back(child);
}

bool Object::RemoveChild(Object* child)
{
	bool removed = false;
	auto foundObj = std::find(children.begin(), children.end(), child);
	if (foundObj != children.end())
	{
		removed = true;
		children.erase(foundObj);
	}
	return removed;
}

bool Object::ContainsChild(Object* child) const
{
	return std::find(children.begin(), children.end(), child) != children.end();
}

void Object::AddToHierarchy(Object* root, Object* obj)
{
	assert(root != nullptr && obj != nullptr);
	root->AddChild(obj);
	obj->SetParent(root);
}

void Object::RemoveFromHierarchy(Object* obj)
{
	if (obj->parent != nullptr)
	{
		obj->parent->RemoveChild(obj);
		obj->SetParent(nullptr);
	}
}
