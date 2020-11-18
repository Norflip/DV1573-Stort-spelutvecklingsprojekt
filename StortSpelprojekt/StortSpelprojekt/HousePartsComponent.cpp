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

	dx::XMMATRIX objOffTrans = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetT());
	dx::XMMATRIX objOffRot = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetR());

	dx::XMMATRIX wepWorld = objOffRot * objOffTrans * skeletonMatrix * objectWorldMatrix;

	dx::XMMatrixDecompose(&objectScale, &objectRot, &objectPos, wepWorld);

	GetOwner()->GetTransform().SetPosition(objectPos);
	GetOwner()->GetTransform().SetRotation(objectRot);
	GetOwner()->GetTransform().SetScale(objectScale);
}
