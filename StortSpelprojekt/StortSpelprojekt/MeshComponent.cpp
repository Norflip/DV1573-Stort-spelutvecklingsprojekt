#include "stdafx.h"
#include "MeshComponent.h"

MeshComponent::MeshComponent(Mesh* mesh, Material* material, Bounds bounds) : bounds(bounds), instanced(false)
{
	meshes.push_back(mesh);
	materials.push_back(material);
}

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
			renderer->Draw(meshes[i], materials[i], GetOwner()->GetTransform().GetWorldMatrix());
	}
}

void MeshComponent::DrawInstanced(Renderer* renderer, CameraComponent* camera) const
{

	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL))
	{
		for (size_t i = 0; i < meshes.size(); i++)
			renderer->DrawInstanced(meshes[i], instanceData.size(), instanceBuffer, materials[i]);
	}
	else
	{
		for (size_t i = 0; i < meshes.size(); i++)
		{
			D3D11_MAPPED_SUBRESOURCE mappedData;
			ZeroMemory(&mappedData, sizeof(D3D11_MAPPED_SUBRESOURCE));

			renderer->GetContext()->Map(instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
			Mesh::InstanceData* dataView = reinterpret_cast<Mesh::InstanceData*>(mappedData.pData);
			size_t instanceCount = 0;

			for (size_t i = 0; i < instanceData.size(); i++) //cull all the instances
			{
				if (camera->InView(bounds, dx::XMLoadFloat4x4(&instanceData[i].instanceWorld))) //the bounding box is in local space so it's same for every instance.
				{
					dataView[instanceCount] = instanceData[i];
					instanceCount++;
				}
			}

			renderer->GetContext()->Unmap(instanceBuffer, 0);
			//meshes[i]->SetInstanceNr(instances);

			if (instanceCount > 0)
			{
				renderer->DrawInstanced(meshes[i], instanceCount, instanceBuffer, materials[i]);
			}
		}
	}
}
