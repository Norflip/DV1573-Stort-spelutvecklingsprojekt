#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh mesh, Material material) : boundingBoxes(mesh)
{
	meshes.push_back(mesh);
	materials.push_back(material);
	boundingBoxes.CalcAABB();
}

MeshComponent::MeshComponent(std::vector<Mesh> meshes, std::vector<Material> materials) : meshes(meshes), materials(materials), boundingBoxes(meshes[0])
{

}

MeshComponent::~MeshComponent() {}

void MeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{

	dx::XMFLOAT3 tmpPos;
	dx::XMStoreFloat3(&tmpPos, GetOwner()->GetTransform().GetWorldPosition());

	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL) || !camera->CullAgainstAABB(boundingBoxes.GetAABB(), tmpPos))
	{
		for (size_t i = 0; i < meshes.size(); i++)
			renderer->Draw(meshes[i], materials[i], GetOwner()->GetTransform().GetWorldMatrix(), *camera);
	}
}
