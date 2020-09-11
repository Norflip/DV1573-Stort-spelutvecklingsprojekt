#include "Object.h"

Object::Object() : material(Shader()), flags(ObjectFlag::ENABLED | ObjectFlag::VISIBLE)
{

}

Object::~Object()
{

}

void Object::Draw(Renderer* renderer, Camera* camera)
{
	renderer->Draw(this->mesh, transform.GetWorldMatrix(), camera->GetViewMatrix(), camera->GetProjectionMatrix());
	// run down the hierarchy
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
