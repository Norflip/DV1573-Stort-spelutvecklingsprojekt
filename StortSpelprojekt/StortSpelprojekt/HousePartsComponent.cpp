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
	dx::XMVECTOR offset({ 3.0f, 2.0f, 2.51f });

	dx::XMMATRIX objectWorldMatrix = skeleton->GetOwner()->GetTransform().GetWorldMatrix();
	dx::XMMATRIX skeletonMatrix = dx::XMLoadFloat4x4(&skeleton->GetFinalTransforms()[0]);
	skeletonMatrix = dx::XMMatrixTranspose(skeletonMatrix);

	dx::XMMATRIX objOffTrans = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetT());
	dx::XMMATRIX objOffRot = dx::XMMatrixTranslationFromVector(skeleton->GetMesh()->GetR());

	dx::XMMATRIX wepWorld = objOffRot * objOffTrans * skeletonMatrix * objectWorldMatrix;

	dx::XMMatrixDecompose(&objectScale, &objectRot, &objectPos, wepWorld);

	if (GetOwner()->HasComponent<RigidBodyComponent>())
	{
		GetOwner()->GetComponent<RigidBodyComponent>()->SetRotation(objectRot);
		GetOwner()->GetComponent<RigidBodyComponent>()->SetPosition(dx::XMVectorAdd(objectPos, offset));

		/*GetOwner()->GetTransform().SetPosition(dx::XMVectorSubtract(objectPos, offset));
		GetOwner()->GetTransform().SetRotation(objectRot);
		GetOwner()->GetTransform().SetScale(objectScale);*/
		/*std::cout << "RB POS: " << GetOwner()->GetComponent<RigidBodyComponent>()->GetPosition().m128_f32[0] << " " << GetOwner()->GetComponent<RigidBodyComponent>()->GetPosition().m128_f32[1] <<
			" " << GetOwner()->GetComponent<RigidBodyComponent>()->GetPosition().m128_f32[2] << std::endl;*/
	}
	else
	{
		GetOwner()->GetTransform().SetPosition(objectPos);
		GetOwner()->GetTransform().SetRotation(objectRot);
		GetOwner()->GetTransform().SetScale(objectScale);
	}
		
}
