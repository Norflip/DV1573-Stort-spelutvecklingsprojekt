#include "Object.h"
#include "Component.h"

size_t Object::idCounter = 0;


Object::Object(const std::string& name, ObjectFlag flag) : name(name), flags(flag), transform(this), id(idCounter++)
{

}

Object::~Object()
{
	for (auto i = components.begin(); i < components.end(); i++)
		delete (*i);

	auto children = transform.GetChildren();
	for (auto i = children.begin(); i < children.end(); i++)
		delete (*i)->GetOwner();

	components.clear();
}

void Object::Update(const float& deltaTime)
{
	if (HasFlag(ObjectFlag::ENABLED))
	{
		transform.MarkNotChanged();

		for (auto i = components.begin(); i < components.end(); i++)
			(*i)->Update(deltaTime);

		auto children = transform.GetChildren();
		for (auto i = children.begin(); i < children.end(); i++)
			(*i)->GetOwner()->Update(deltaTime);
	}
}

void Object::FixedUpdate(const float& fixedDeltaTime)
{
	if (HasFlag(ObjectFlag::ENABLED))
	{
		for (auto i = components.begin(); i < components.end(); i++)
			(*i)->FixedUpdate(fixedDeltaTime);

		auto children = transform.GetChildren();
		for (auto i = children.begin(); i < children.end(); i++)
			(*i)->GetOwner()->FixedUpdate(fixedDeltaTime);
	}
}

void Object::Draw(Renderer* renderer, CameraComponent* camera)
{
	if (HasFlag(ObjectFlag::ENABLED))
	{

		for (auto i = components.begin(); i < components.end(); i++)
			(*i)->Draw(renderer, camera);

		auto children = transform.GetChildren();
		for (auto i = children.begin(); i < children.end(); i++)
			(*i)->GetOwner()->Draw(renderer, camera);	
		
	}
}

bool Object::HasFlag(ObjectFlag flag) const
{
	return ((int)flag & (int)flags) != 0;
}

void Object::AddFlag(ObjectFlag flag)
{
	flags = static_cast<ObjectFlag>((int)flags | (int)flag);
}

void Object::RemoveFlag(ObjectFlag flag)
{
	flags = static_cast<ObjectFlag>((int)flags & ~((int)flag));
}
