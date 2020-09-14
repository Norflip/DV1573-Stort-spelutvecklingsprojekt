#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material) {}
MeshComponent::~MeshComponent() {}

void MeshComponent::Update(const float& deltaTime)
{
	float rotationDegree = 0.4f * deltaTime;
	GetOwner()->GetTransform().Rotate(rotationDegree, rotationDegree, rotationDegree);
}

void MeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
	material.BindToContext(renderer->GetContext());

	auto view = camera->GetViewMatrix();
	auto proj = camera->GetProjectionMatrix();

	renderer->Draw(this->mesh, GetOwner()->GetTransform().GetWorldMatrix(), camera->GetViewMatrix(), camera->GetProjectionMatrix());
}
