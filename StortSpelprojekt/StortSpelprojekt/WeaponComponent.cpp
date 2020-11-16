#include "stdafx.h"
#include "WeaponComponent.h"
#include "SkeletonAni.h"

WeaponComponent::WeaponComponent(SkeletonMeshComponent* meshComp)
	:skeleton(meshComp)
{
}

void WeaponComponent::Initialize()
{
	//up = { 0.0f, 1.0f, 1.0f };
}

void WeaponComponent::Update(const float& deltaTime)
{
	SetPosition(deltaTime);
}

void WeaponComponent::SetPosition(float time)
{
	dx::XMVECTOR weaponPos;
	dx::XMVECTOR weaponRot;
	dx::XMVECTOR weaponScale;

	dx::XMMATRIX playerWorldMatrix = skeleton->GetOwner()->GetTransform().GetWorldMatrix();
	dx::XMMATRIX skeletonMatrix = dx::XMLoadFloat4x4(&skeleton->GetFinalTransforms()[21]);
	skeletonMatrix = dx::XMMatrixTranspose(skeletonMatrix);

	dx::XMMATRIX wepOffTrans = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetT());
	dx::XMMATRIX wepOffRot = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetR());
	
	dx::XMMATRIX wepWorld = wepOffRot * wepOffTrans * skeletonMatrix * playerWorldMatrix;

	dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, wepWorld);

	GetOwner()->GetTransform().SetPosition(weaponPos);
	GetOwner()->GetTransform().SetRotation(weaponRot);
	GetOwner()->GetTransform().SetScale(weaponScale);
}
