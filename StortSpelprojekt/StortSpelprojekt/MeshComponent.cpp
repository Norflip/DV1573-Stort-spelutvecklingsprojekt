#include "stdafx.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh* mesh, Material* material) : bounds(), instanced(false)
{
	meshes.push_back(mesh);
	materials.push_back(material);
	bounds.CalculateAABB(meshes);
}

MeshComponent::MeshComponent(std::vector<Mesh*> meshes, std::vector<Material*> materials) 
	: meshes(meshes), materials(materials), bounds(), instanced(false)
{
	bounds.CalculateAABB(meshes);
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

	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL) || camera->InView(bounds, GetOwner()->GetTransform().GetWorldMatrix()))
	{
		for (size_t i = 0; i < meshes.size(); i++)
			renderer->Draw(meshes[i], materials[i], GetOwner()->GetTransform().GetWorldMatrix(), camera);
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
			D3D11_MAPPED_SUBRESOURCE mappedData = DXHelper::BindInstanceBuffer(renderer->GetContext(), instanceBuffer);
			Mesh::InstanceData* dataView = reinterpret_cast<Mesh::InstanceData*>(mappedData.pData);

			for (size_t instance = 0; instance < instanceData.size(); instance++) //cull all the instances
			{
				if (camera->InView(bounds, dx::XMLoadFloat4x4(&instanceData[instance].instanceWorld))) //the bounding box is in local space so it's same for every instance.
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
