#include "WeaponComponent.h"

WeaponComponent::WeaponComponent(Object* object)
	:camObj(object)
{
}

void WeaponComponent::Initialize()
{
	SetPosition();
}

void WeaponComponent::Update(const float& deltaTime)
{
	UpdatePosition();
}

void WeaponComponent::SetPosition()
{
	weaponPos = camObj->GetTransform().GetPosition();
	weaponPos.m128_f32[2] += 0.7f;
	weaponPos.m128_f32[1] -= 0.2f;
	weaponPos.m128_f32[0] += 0.2f;

	weaponRot = camObj->GetTransform().GetRotation();
	weaponRot.m128_f32[2] -= 0.55f;
	weaponRot.m128_f32[0] += 0.3f;
	weaponRot.m128_f32[1] -= 0.3f;

	GetOwner()->GetTransform().SetPosition(weaponPos);
	GetOwner()->GetTransform().SetRotation(weaponRot);
}

void WeaponComponent::UpdatePosition()
{
	GetOwner()->GetTransform().SetParent(&camObj->GetTransform());
}
