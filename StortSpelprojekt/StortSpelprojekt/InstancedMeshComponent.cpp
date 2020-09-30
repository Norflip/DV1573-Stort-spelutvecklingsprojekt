#include "InstancedMeshComponent.h"

InstancedMeshComponent::InstancedMeshComponent(Mesh mesh, Material material) : boundingBoxes(mesh)
{
	meshes.push_back(mesh);
	materials.push_back(material);
	boundingBoxes.CalcAABB();
}

InstancedMeshComponent::~InstancedMeshComponent()
{
}

void InstancedMeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{

	D3D11_MAPPED_SUBRESOURCE mappedData = {}; //zeroing out the memory like this.
	UINT instances = 0;
	meshes[0].SetInstanceNr(instances);
	//IF INSTANCES ARE PARENTED THEY NEED TO HAVE THEIR POSITION UPDATED PROBABLY INSIDE THE TRANSFORM CLASS
	renderer->GetContext()->Map(meshes[0].instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData); //the instance buffer is dynmaic so you can update it.

	Mesh::InstanceData* dataView = reinterpret_cast<Mesh::InstanceData*>(mappedData.pData); //a safe cast
	dx::XMFLOAT3 tmpPos;
	for (UINT instance = 0; instance < (UINT)meshes[0].instanceData.size(); instance++) //cull all the instances
	{
		tmpPos = meshes[0].instanceData[instance].instancePosition; //If the position is depending on a parent then update this variable with that parent. This is for static meshes.

		if (!camera->CullAgainstAABB(boundingBoxes.GetAABB(), tmpPos)) //the bounding box is in local space so it's same for every instance.
		{
			dataView[instances++] = meshes[0].instanceData[instance];
		}

		renderer->GetContext()->Unmap(meshes[0].instanceBuffer, 0);

		meshes[0].SetInstanceNr(instances);
		if (instances > 0)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				renderer->Draw(meshes[i], materials[i], GetOwner()->GetTransform().GetWorldMatrix(), *camera);
		}

	}



}

InstancedMeshComponent::InstancedMeshComponent(std::vector<Mesh> meshes, std::vector<Material> materials) : meshes(meshes), materials(materials), boundingBoxes(meshes[0])

{
}







