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
	MeshComponent(Mesh mesh, Material material);
	MeshComponent(std::vector <Mesh> meshes, std::vector <Material> materials);
	virtual ~MeshComponent();
	
	std::vector<Mesh> GetMesh() const { return this->meshes; }
	std::vector<Material> GetMaterial() const { return this->materials; }

	void Draw(Renderer* renderer, CameraComponent* camera) override;
	BoundingBoxes& GetBoundingBoxes() { return this->boundingBoxes; }

	ALIGN16_ALLOC;

private:
	std::vector<Mesh> meshes;
	std::vector<Material> materials;
	BoundingBoxes boundingBoxes;
};