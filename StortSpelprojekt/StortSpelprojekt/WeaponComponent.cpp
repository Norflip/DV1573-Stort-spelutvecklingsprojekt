#include "WeaponComponent.h"

WeaponComponent::WeaponComponent(Object* object)
	:camObj(object)
{
	
}

void WeaponComponent::Initialize()
{
	SetPosition();
}

void WeaponComponent::FixedUpdate(const float& fixedDeltaTime)
{

}

void WeaponComponent::Update(const float& deltaTime)
{
	SetPosition();
}

void WeaponComponent::SetPosition()
{

	//GetOwner()->GetTransform().SetParent(&camObj->GetTransform());
	//GetOwner()->GetTransform().SetParentChild(camObj->GetTransform(), GetOwner()->GetTransform());

	
	//weaponPos = camObj->GetTransform().GetPosition();

	//weaponPos.m128_f32[2] += 0.5f;

	//weaponRot = camObj->GetTransform().GetRotation();

	//GetOwner()->GetTransform().SetPosition(weaponPos);

	//dx::XMMatrixRotationAxis(weaponPos, 100);
	//GetOwner()->GetTransform().SetRotation(weaponRot);

	GetOwner()->GetTransform().SetParent(&camObj->GetTransform());
	dx::XMStoreFloat4x4(&weaponMatrix, GetOwner()->GetTransform().GetLocalWorldMatrix() * dx::XMMatrixInverse(NULL, camObj->GetComponent<CameraComponent>()->GetViewMatrix()));

	dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, GetOwner()->GetTransform().GetLocalWorldMatrix() * dx::XMMatrixInverse(NULL, camObj->GetComponent<CameraComponent>()->GetViewMatrix()));
	
	GetOwner()->GetTransform().SetPosition({ weaponPos.m128_f32[0], weaponPos.m128_f32[1], weaponPos.m128_f32[2] });


	GetOwner()->GetTransform().SetRotation({ weaponPos.m128_f32[3], weaponPos.m128_f32[4], weaponPos.m128_f32[5] });


}
