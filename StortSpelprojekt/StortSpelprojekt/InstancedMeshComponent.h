#pragma once
#include "Component.h"
#include "Material.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "BoundingBoxes.h"
#include "Object.h"
class InstancedMeshComponent : public Component
{

public:
	InstancedMeshComponent(Mesh mesh, Material material);
	InstancedMeshComponent(std::vector <Mesh> meshes, std::vector <Material> materials);
	virtual ~InstancedMeshComponent();

	std::vector<Mesh> GetMesh() const { return this->meshes; }
	std::vector<Material> GetMaterial() const { return this->materials; }

	void Draw(Renderer* renderer, CameraComponent* camera) override;
	BoundingBoxes& GetBoundingBoxes() { return this->boundingBoxes; }
	
private:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	BoundingBoxes boundingBoxes;
};

