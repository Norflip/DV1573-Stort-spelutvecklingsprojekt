#include "stdafx.h"
#include "WeaponComponent.h"
#include "SkeletonAni.h"

WeaponComponent::WeaponComponent(SkeletonMeshComponent* meshComp)
	:weaponPos({ 0,0,0 }), weaponRot({ 0,0,0 }), weaponScale({ 0,0,0 }), up({ 0,0,0 }), skeleton(meshComp)
{
}

void WeaponComponent::Initialize()
{
	up = { 0.0f, 1.0f, 1.0f };
}

void WeaponComponent::Update(const float& deltaTime)
{
	SetPosition(deltaTime);
}

void WeaponComponent::SetPosition(float time)
{
	playerWorldMatrix = skeleton->GetOwner()->GetTransform().GetWorldMatrix();
	
	skeletonMatrix = dx::XMLoadFloat4x4(&skeleton->GetFinalTransforms()[21]);
	skeletonMatrix = dx::XMMatrixTranspose(skeletonMatrix);

	wepOffTrans = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetT());
	wepOffRot = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetR());
	
	wepWorld = wepOffRot * wepOffTrans * skeletonMatrix * playerWorldMatrix;
	dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, wepWorld);

	GetOwner()->GetTransform().SetPosition(weaponPos);
	GetOwner()->GetTransform().SetRotation(weaponRot);
	GetOwner()->GetTransform().SetScale(weaponScale);
}
