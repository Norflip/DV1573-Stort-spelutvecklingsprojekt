#include "Object.h"
#include "Component.h"

Object::Object(const std::string& name, ObjectFlag flag) : name(name), flags(flag), transform(this)
{

}

Object::~Object()
{

}

void Object::Update(const float& deltaTime)
{
	for (auto i = components.begin(); i < components.end(); i++)
	{
		(*i)->Update(deltaTime);
	}
}

void Object::Draw(Renderer* renderer, Camera* camera)
{
	for (auto i = components.begin(); i < components.end(); i++)
	{
		(*i)->Draw(renderer, camera);
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
