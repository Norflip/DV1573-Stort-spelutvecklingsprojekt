#pragma once
#include "Component.h"
#include "Material.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "Object.h"
#include "BoundingBoxes.h"

class MeshComponent : public Component
{
public:
	MeshComponent(Mesh mesh, Material material);
	virtual ~MeshComponent();
	
	Mesh GetMesh() const { return this->mesh; }
	Material GetMaterial() const { return this->material; }

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;
	BoundingBoxes& GetBoundingBoxes() { return this->boundingBoxes; }

private:
	Mesh mesh;
	Material material;
	BoundingBoxes boundingBoxes;
};