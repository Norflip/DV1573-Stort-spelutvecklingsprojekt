#pragma once
#include <vector>

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Renderer.h"
#include "Camera.h"

enum class ObjectFlag : unsigned int
{
	NONE = 0,
	ENABLED = 1 << 0,
	VISIBLE = 1 << 1,
	STATIC = 1 << 2,

	DEFAULT = ENABLED | VISIBLE
};

DEFINE_ENUM_FLAG_OPERATORS(ObjectFlag)

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
	
	bool HasFlag(ObjectFlag flag) const;
	void AddFlag(ObjectFlag flag);
	void RemoveFlag(ObjectFlag flag);
	
	Transform& GetTransform() { return this->transform; }

private:
	ObjectFlag flags;
	Transform transform;
	Mesh mesh;
	Material material;

};