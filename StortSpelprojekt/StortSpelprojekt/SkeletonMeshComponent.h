#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "BoundingBoxes.h"
#include "Object.h"


enum SkeletonStateMachine
{
	IDLE,
	WALK,
	RUN,
	ATTACK,
	NONE,
	DEATH
};


class SkeletonMeshComponent : public Component
{
public:
	SkeletonMeshComponent(Mesh mesh, Material material);
	virtual ~SkeletonMeshComponent();

	Mesh GetMesh() const { return this->mesh; }
	Material GetMaterial() const { return this->material; }

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;
	void RunAnimation(const float& deltaTime);
	void SetAnimationTrack(const SkeletonAni& skeletonAni, const SkeletonStateMachine& type);

	SkeletonAni& GetAnimationTrack(unsigned int trackNr);

	void Play(const SkeletonStateMachine& type);
	
private:
	Mesh mesh;
	Material material;
	std::vector<SkeletonAni> skeletonAnimations;
	float elapsedTime = 0.0f;
	std::unordered_map<SkeletonStateMachine, unsigned int> trackMap;
	std::vector<dx::XMFLOAT4X4> finalTransforms;
	SkeletonStateMachine currentAni = SkeletonStateMachine::NONE;
	BoundingBoxes boundingBoxes;
	float componentDeltaTime = 0.0f;
	
};

