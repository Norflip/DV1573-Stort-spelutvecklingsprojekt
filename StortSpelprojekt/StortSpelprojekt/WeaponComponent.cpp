#include "stdafx.h"
#include "WeaponComponent.h"
#include "SkeletonAni.h"

WeaponComponent::WeaponComponent(Object* object, SkeletonMeshComponent* skeletonMeshComp)
	:camObj(object), weaponPos({ 0,0,0 }), weaponRot({ 0,0,0 }), weaponScale({ 0,0,0 }), up({ 0,0,0 })
{
	//Joint nr 21 är den hand som yxan ska parentas till
}

void WeaponComponent::Initialize()
{
	camComp = camObj->GetComponent<CameraComponent>();
	up = { 0.0f, 1.0f, 1.0f };
}

void WeaponComponent::Update(const float& deltaTime)
{
	SetPosition(deltaTime);
}

void WeaponComponent::SetPosition(float time)
{

	inverseViewMatrix = dx::XMMatrixInverse(nullptr, camComp->GetViewMatrix());
	wepOffTrans = dx::XMMatrixTranslation(0.3f, -0.4f, 0.8f);
	wepOffRot = dx::XMMatrixRotationAxis(up, dx::XMConvertToRadians(-40.0f));
	
	wepWorld = wepOffRot * wepOffTrans * inverseViewMatrix;
	dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, wepWorld);

	GetOwner()->GetTransform().SetPosition(weaponPos);
	GetOwner()->GetTransform().SetRotation(weaponRot);
	GetOwner()->GetTransform().SetScale(weaponScale);

}
