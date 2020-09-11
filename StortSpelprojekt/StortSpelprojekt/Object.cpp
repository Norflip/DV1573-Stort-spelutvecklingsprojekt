#include "Object.h"

Object::Object() : material(Shader()), enabled(true)
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
