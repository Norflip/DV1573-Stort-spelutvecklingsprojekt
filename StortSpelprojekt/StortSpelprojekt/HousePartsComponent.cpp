#include "stdafx.h"
#include "HousePartsComponent.h"



HousePartsComponent::HousePartsComponent(SkeletonMeshComponent* meshComp)
	:skeleton(meshComp)
{
}

void HousePartsComponent::Initialize()
{
}

void HousePartsComponent::Update(const float& deltaTime)
{
	SetPosition(deltaTime);
}

void HousePartsComponent::SetPosition(float time)
{
	dx::XMVECTOR objectPos;
	dx::XMVECTOR objectRot;
	dx::XMVECTOR objectScale;

	dx::XMMATRIX objectWorldMatrix = skeleton->GetOwner()->GetTransform().GetWorldMatrix();
	dx::XMMATRIX skeletonMatrix = dx::XMLoadFloat4x4(&skeleton->GetFinalTransforms()[0]);
	skeletonMatrix = dx::XMMatrixTranspose(skeletonMatrix);

	dx::XMMATRIX objOff = skeleton->GetMesh()->GetTransformMatrix();

	dx::XMMATRIX inWorld = objOff * skeletonMatrix * objectWorldMatrix;

	dx::XMMatrixDecompose(&objectScale, &objectRot, &objectPos, inWorld);

	GetOwner()->GetTransform().SetWorldPosition(objectPos);
	GetOwner()->GetTransform().SetWorldRotation(objectRot);
}
