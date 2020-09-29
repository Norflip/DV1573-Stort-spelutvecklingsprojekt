#include "SkeletonMeshComponent.h"

SkeletonMeshComponent::SkeletonMeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material), elapsedTime(0.0f)
{
}

SkeletonMeshComponent::~SkeletonMeshComponent()
{
}

void SkeletonMeshComponent::Update(const float& deltaTime)
{
    elapsedTime += deltaTime;
}

void SkeletonMeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
	material.BindToContext(renderer->GetContext());
	auto bones = GetAnimationTrack(0).Makeglobal(elapsedTime, dx::XMMatrixIdentity(), GetAnimationTrack(0).GetRootKeyJoints());
	renderer->DrawSkeleton(mesh,material, GetOwner()->GetTransform().GetWorldMatrix(), *camera, bones);
}

void SkeletonMeshComponent::SetAnimationTrack(const SkeletonAni& skeletonAni)
{
	skeletonAnimations.push_back(skeletonAni);
}

SkeletonAni& SkeletonMeshComponent::GetAnimationTrack(unsigned int trackNr)
{
	return skeletonAnimations[trackNr];
}
