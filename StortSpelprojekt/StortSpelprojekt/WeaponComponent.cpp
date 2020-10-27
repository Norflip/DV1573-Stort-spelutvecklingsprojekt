#include "WeaponComponent.h"

WeaponComponent::WeaponComponent(Object* object)
	:camObj(object), weaponPos({ 0,0,0 }), weaponRot({ 0,0,0 }), weaponScale({ 0,0,0 }), up({ 0,0,0 })
{
}

void WeaponComponent::Initialize()
{
	up = { 0.0f, 1.0f, 0.0f };
	SetPosition();
}

void WeaponComponent::Update(const float& deltaTime)
{
	SetPosition();
}

void WeaponComponent::SetPosition()
{
	inverseViewMatrix = dx::XMMatrixInverse(nullptr, camObj->GetComponent<CameraComponent>()->GetViewMatrix());
	wepOffTrans = dx::XMMatrixTranslation(0.0f, 0.0f, 0.5f);
	wepOffRot = dx::XMMatrixRotationAxis(up, dx::XMConvertToRadians(0));
	wepWorld = wepOffRot * wepOffTrans * inverseViewMatrix;

	dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, wepWorld);

	GetOwner()->GetTransform().SetPosition(weaponPos);
	GetOwner()->GetTransform().SetRotation(weaponRot);
	GetOwner()->GetTransform().SetScale(weaponScale);
}
