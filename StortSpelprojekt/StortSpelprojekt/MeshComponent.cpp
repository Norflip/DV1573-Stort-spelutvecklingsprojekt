#include "stdafx.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh* mesh, Material* material) : boundingBoxes(mesh), instanced(false)
{
	meshes.push_back(mesh);
	materials.push_back(material);
	boundingBoxes.CalcAABB();

}

MeshComponent::MeshComponent(std::vector<Mesh*> meshes, std::vector<Material*> materials) 
	: meshes(meshes), materials(materials), boundingBoxes(meshes[0]), instanced(false)
{

}

MeshComponent::~MeshComponent() 
{
	RELEASE(instanceBuffer);
}

void MeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
	if (IsInstanced())
	{
		DrawInstanced(renderer, camera);
	}
	else
	{
		DrawNonInstanced(renderer, camera);
	}
}

void MeshComponent::SetInstanceable(size_t index, std::vector<Mesh::InstanceData> instanceData, size_t instanceCount, ID3D11Device* device)
{
	//Initialize(device);
	this->instanceCount = instanceCount;
	this->instanceData = instanceData;
	this->instanced = true;

	DXHelper::CreateInstanceBuffer(device, instanceData.size(), sizeof(Mesh::InstanceData), instanceData.data(), &instanceBuffer);
	assert(instanceBuffer != nullptr);
}

void MeshComponent::DrawNonInstanced(Renderer* renderer, CameraComponent* camera) const
{
	dx::XMFLOAT3 tmpPos;
	dx::XMStoreFloat3(&tmpPos, GetOwner()->GetTransform().GetWorldPosition());

	if (!camera->CullAgainstAABB(boundingBoxes.GetAABB(), tmpPos))
	{
		for (size_t i = 0; i < meshes.size(); i++)
			renderer->Draw(meshes[i], materials[i], GetOwner()->GetTransform().GetWorldMatrix(), camera);
	}
	else
	{
		std::cout << (GetOwner()->GetName() + " CULLED") << std::endl;
	}
}

void MeshComponent::DrawInstanced(Renderer* renderer, CameraComponent* camera) const
{
	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL))
	{
		for (size_t i = 0; i < meshes.size(); i++)
			renderer->DrawInstanced(meshes[i], instanceData.size(), instanceBuffer,  materials[i], camera);
	}
	else
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			size_t instanceCount = 0;
			dx::XMFLOAT3 tmpPos;

			D3D11_MAPPED_SUBRESOURCE mappedData = DXHelper::BindInstanceBuffer(renderer->GetContext(), instanceBuffer);
			Mesh::InstanceData* dataView = reinterpret_cast<Mesh::InstanceData*>(mappedData.pData);

			for (size_t instance = 0; instance < instanceData.size(); instance++) //cull all the instances
			{
				tmpPos = instanceData[instance].instancePosition; //If the position is depending on a parent then update this variable with that parent. This is for static meshes.

				if (!camera->CullAgainstAABB(boundingBoxes.GetAABB(), tmpPos)) //the bounding box is in local space so it's same for every instance.
				{
					dataView[instanceCount++] = instanceData[instance];
				}
			}

			DXHelper::UnBindInstanceBuffer(renderer->GetContext(), instanceBuffer);
			//meshes[i]->SetInstanceNr(instances);

			if (instanceCount > 0)
			{
				renderer->DrawInstanced(meshes[i], instanceCount, instanceBuffer, materials[i], camera);
			}
		}
	}
}
