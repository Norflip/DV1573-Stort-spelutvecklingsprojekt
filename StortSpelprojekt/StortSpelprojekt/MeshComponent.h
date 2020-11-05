#pragma once
#include "Component.h"
#include "Material.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "BoundingBoxes.h"
#include "Object.h"

ALIGN16
class MeshComponent : public Component
{
public:
	MeshComponent(Mesh* mesh, Material* material);
	MeshComponent(std::vector <Mesh*> meshes, std::vector <Material*> materials);
	virtual ~MeshComponent();
	
	std::vector<Mesh*> GetMeshes() const { return this->meshes; }
	std::vector<Material*> GetMaterials() const { return this->materials; }

	void Draw(Renderer* renderer, CameraComponent* camera) override;
	BoundingBox& GetBoundingBoxes() { return this->bounds; }
	
	void SetInstanceable(size_t index, std::vector<Mesh::InstanceData> instanceData, size_t instanceCount, ID3D11Device* device);
	ID3D11Buffer* GetInstanceBuffer() const { return this->instanceBuffer; }

	bool IsInstanced() const { return this->instanced; }
	size_t InstanceCount() const { return this->instanceCount; }

	size_t GetInstanceNr() const { return this->instanceCount; }
	void SetInstanceNr(size_t count) { this->instanceCount = count; }
	std::vector<Mesh::InstanceData>& GetInstanceData() { return this->instanceData; }

	ALIGN16_ALLOC;

private:
	void DrawNonInstanced(Renderer* renderer, CameraComponent* camera) const;
	void DrawInstanced(Renderer* renderer, CameraComponent* camera) const;

private:
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	BoundingBox bounds;

	std::vector<Mesh::InstanceData> instanceData;
	ID3D11Buffer* instanceBuffer;
	bool instanced;
	size_t instanceCount;

};