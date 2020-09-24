#include "SkeletonMeshComponent.h"

SkeletonMeshComponent::SkeletonMeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material)
{
}

SkeletonMeshComponent::~SkeletonMeshComponent()
{
}

void SkeletonMeshComponent::Update(const float& deltaTime)
{
    elapsedTime = deltaTime;
}

void SkeletonMeshComponent::Draw(Renderer* renderer, CameraComponent* camera, DrawType drawType)
{

	material.BindToContext(renderer->GetContext());

	/*renderer->DrawSkeleton(mesh, GetOwner()->GetTransform().GetWorldMatrix(), camera->GetViewMatrix(), camera->GetProjectionMatrix(),
		GetAnimationTrack(0).Makeglobal(elapsedTime, dx::XMMatrixIdentity(), GetAnimationTrack(0).GetRootKeyJoints()));*/


}

void SkeletonMeshComponent::SetAnimationTrack(const SkeletonAni& skeletonAni)
{
	skeletonAnimations.push_back(skeletonAni);
}

SkeletonAni& SkeletonMeshComponent::GetAnimationTrack(unsigned int trackNr)
{
	return skeletonAnimations[trackNr];
}
