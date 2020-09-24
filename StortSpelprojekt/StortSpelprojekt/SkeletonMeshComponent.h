#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "Object.h"
class SkeletonMeshComponent : public Component
{
public:
	SkeletonMeshComponent(Mesh mesh, Material material);
	virtual ~SkeletonMeshComponent();

	Mesh GetMesh() const { return this->mesh; }
	Material GetMaterial() const { return this->material; }

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera, DrawType drawType) override;

	void SetAnimationTrack(const SkeletonAni& skeletonAni);

	SkeletonAni& GetAnimationTrack(unsigned int trackNr);
	
private:
	Mesh mesh;
	Material material;
	std::vector<SkeletonAni> skeletonAnimations;
	float elapsedTime;
};

