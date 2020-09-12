#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "Camera.h"
#include "Object.h"

class MeshComponent : public Component
{
public:
	MeshComponent(Mesh mesh, Material material);
	virtual ~MeshComponent();
	
	Mesh GetMesh() const { return this->mesh; }
	Material GetMaterial() const { return this->material; }

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, Camera* camera) override;

private:
	Mesh mesh;
	Material material;

};