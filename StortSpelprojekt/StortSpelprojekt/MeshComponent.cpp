#include "MeshComponent.h"
#include "MoveComponent.h"

MeshComponent::MeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material),boundingBoxes(mesh) {}
MeshComponent::~MeshComponent() {}

void MeshComponent::Update(const float& deltaTime)
{
	if (GetOwner()->GetComponent<MoveComponent>())
	{
		float rotationDegree = 0.4f * deltaTime;
		GetOwner()->GetTransform().Rotate(0.0f, rotationDegree, 0.0f);
	}
}

void MeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
	renderer->Draw(mesh, material,  GetOwner()->GetTransform().GetWorldMatrix(), *camera);
}
