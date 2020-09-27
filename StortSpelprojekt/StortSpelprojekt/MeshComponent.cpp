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
	dx::XMFLOAT3 tmpPos;
	dx::XMStoreFloat3(&tmpPos, GetOwner()->GetTransform().GetPosition());

	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL) || !camera->CullAgainstAABB(boundingBoxes.GetAABB(), tmpPos))
	{
		renderer->Draw(mesh, material, GetOwner()->GetTransform().GetWorldMatrix(), *camera);
	}
}
