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
	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL))
	{
		for (size_t i = 0; i < meshes.size(); i++)
			renderer->DrawInstanced(meshes[i], meshes[i].GetInstanceNr(), materials[i], *camera);
	}
	else
	{
		
		UINT instances = 0;

		

		Mesh::InstanceData* dataView = nullptr;
		
		dx::XMFLOAT3 tmpPos;
		
		D3D11_MAPPED_SUBRESOURCE mappedData = DXHelper::BindInstanceBuffer(renderer->GetContext(), meshes[0].instanceBuffer);
		
		 dataView = reinterpret_cast<Mesh::InstanceData*>(mappedData.pData);
		
		for (UINT instance = 0; instance < (UINT)meshes[0].instanceData.size(); instance++) //cull all the instances
		{
			tmpPos = meshes[0].instanceData[instance].instancePosition; //If the position is depending on a parent then update this variable with that parent. This is for static meshes.

			if (!camera->CullAgainstAABB(boundingBoxes.GetAABB(), tmpPos)) //the bounding box is in local space so it's same for every instance.
			{
				dataView[instances++] = meshes[0].instanceData[instance];
			}



		}
		DXHelper::UnBindInstanceBuffer(renderer->GetContext(), meshes[0].instanceBuffer);

		meshes[0].SetInstanceNr(instances);
		if (instances > 0)
		{
			for (size_t i = 0; i < meshes.size(); i++)
				renderer->DrawInstanced(meshes[i], instances, materials[i], *camera);
		}
		
	}
	

}

InstancedMeshComponent::InstancedMeshComponent(std::vector<Mesh> meshes, std::vector<Material> materials) : meshes(meshes), materials(materials), boundingBoxes(meshes[0])

{
}







