#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh mesh, Material material) : 
	mesh(mesh), material(material) {}

MeshComponent::~MeshComponent() {}

void MeshComponent::Update(const float& deltaTime)
{
	GetOwner()->GetTransform().Rotate(2.0f * deltaTime, 2.0f * deltaTime, 2.0f * deltaTime);
}

void MeshComponent::Draw(Renderer* renderer, Camera* camera)
{
	material.BindToContext(renderer->GetContext());
//	renderer->Draw(this->mesh, GetOwner()->GetTransform().GetWorldMatrix(), camera->GetViewMatrix(), camera->GetProjectionMatrix());
	renderer->Draw(this->mesh, dx::XMMatrixIdentity(), camera->GetViewMatrix(), camera->GetProjectionMatrix());

}
