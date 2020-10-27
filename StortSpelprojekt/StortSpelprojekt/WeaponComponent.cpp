#include "WeaponComponent.h"

WeaponComponent::WeaponComponent(Object* object)
	:camObj(object)
{
}

void WeaponComponent::Initialize()
{
	//weaponMatrix = GetOwner()->GetTransform().GetLocalWorldMatrix() * dx::XMMatrixInverse(nullptr, camObj->GetComponent<CameraComponent>()->GetViewMatrix());
	SetPosition();
	
	//camObj->GetTransform().SetParentChild(camObj->GetTransform(), GetOwner()->GetTransform());
	//GetOwner()->GetTransform().SetParentChild(camObj->GetTransform(), GetOwner()->GetTransform());
	//camObj->GetTransform().SetParent(&GetOwner()->GetTransform());
	//GetOwner()->GetTransform().SetParent(&camObj->GetTransform());
	//camObj->GetTransform().AddChild(&GetOwner()->GetTransform());
}

void WeaponComponent::Update(const float& deltaTime)
{

	SetPosition();
}

void WeaponComponent::SetPosition()
{
	weaponPos = camObj->GetTransform().GetPosition();
	weaponPos.m128_f32[0] += 0.2f;
	weaponPos.m128_f32[1] -= 0.2f;
	weaponPos.m128_f32[2] += 0.7f;

	weaponRot = camObj->GetTransform().GetRotation();

	weaponRot.m128_f32[0] += 0.3f;
	weaponRot.m128_f32[1] -= 0.3f;
	weaponRot.m128_f32[2] -= 0.55f;


	forward = { forward.m128_f32[0] * weaponRot.m128_f32[0], forward.m128_f32[1] * weaponRot.m128_f32[1], forward.m128_f32[2] * weaponRot.m128_f32[2] };
	//weaponRot.m128_f32[0] += GetOwner()->GetTransform().GetRotation().m128_f32[0] - camObj->GetTransform().GetRotation().m128_f32[0];
	//weaponRot.m128_f32[1] += GetOwner()->GetTransform().GetRotation().m128_f32[1] - camObj->GetTransform().GetRotation().m128_f32[1];
	//weaponRot.m128_f32[2] += GetOwner()->GetTransform().GetRotation().m128_f32[2] - camObj->GetTransform().GetRotation().m128_f32[2];
	
	GetOwner()->GetTransform().SetPosition(weaponPos);
	GetOwner()->GetTransform().SetRotation(forward);

	//dx::XMMatrixRotationAxis({ GetOwner()->GetTransform().GetPosition().m128_f32[0] - camObj->GetTransform().GetPosition().m128_f32[0],
	//	GetOwner()->GetTransform().GetPosition().m128_f32[1] - camObj->GetTransform().GetPosition().m128_f32[1],
	//	GetOwner()->GetTransform().GetPosition().m128_f32[2] - camObj->GetTransform().GetPosition().m128_f32[2]}, 0);

	//dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, dx::XMMatrixRotationAxis(camObj->GetTransform().GetPosition(), 0));
	
	//weaponMatrix = GetOwner()->GetTransform().GetLocalWorldMatrix() * dx::XMMatrixInverse(nullptr, camObj->GetComponent<CameraComponent>()->GetViewMatrix());
	
	//weaponPos = dx::XMVECTOR({ 0,0,0 });
	//weaponRot = dx::XMVECTOR({ 0,0,0 });
	//weaponScale = dx::XMVECTOR({ 0,0,0 });



	//weaponMatrix = GetOwner()->GetTransform().GetLocalWorldMatrix() *= dx::XMMatrixInverse(nullptr, camObj->GetComponent<CameraComponent>()->GetViewMatrix());

	//dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, weaponMatrix);

	////dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, GetOwner()->GetTransform().GetLocalWorldMatrix() * camObj->GetComponent<CameraComponent>()->GetViewMatrix());

	//std::cout << "POSITION OF AXE:" << weaponPos.m128_f32[0] << " " << weaponPos.m128_f32[1] << " " << weaponPos.m128_f32[2] << std::endl << std::endl;
	//std::cout << "ROTATION OF AXE:" << weaponRot.m128_f32[0] << " " << weaponRot.m128_f32[1] << " " << weaponRot.m128_f32[2] << std::endl << std::endl;

	//weaponPos.m128_f32[0] += 0.2f;
	//weaponPos.m128_f32[1] -= 0.2f;
	//weaponPos.m128_f32[2] += 0.7f;
	//GetOwner()->GetTransform().SetPosition(weaponPos);

	//weaponRot.m128_f32[0] += 0.3f;
	//weaponRot.m128_f32[1] -= 0.3f;
	//weaponRot.m128_f32[2] -= 0.55f;
	//GetOwner()->GetTransform().SetRotation(weaponRot);
	//GetOwner()->GetTransform().SetScale(weaponScale);


}
