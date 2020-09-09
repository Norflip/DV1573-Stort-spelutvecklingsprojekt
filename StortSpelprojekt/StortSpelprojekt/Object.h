#pragma once
#include <vector>

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Renderer.h"
#include "Camera.h"

class Object
{
public:
	Object();
	virtual ~Object();

	void SetMesh(Mesh mesh) { this->mesh = mesh; }
	Mesh GetMesh() const { return this->mesh; }
	
	void SetMaterial(Material material) { this->material = material; }
	Material GetMaterial() const { return this->material; }

	void Draw(Renderer* renderer, Camera* camera);
	void SetEnable(bool enabled) { this->enabled = enabled; }
	bool IsEnabled() const { return this->enabled; }

	Transform& GetTransform() { return this->transform; }

private:
	bool enabled;
	Transform transform;
	Mesh mesh;
	Material material;

};