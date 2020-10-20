#include "SkeletonMeshComponent.h"

SkeletonMeshComponent::SkeletonMeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material), boundingBoxes(mesh)
{
	boundingBoxes.CalcAABB();
	currentAni = SkeletonStateMachine::NONE;
	finalTransforms.resize(60);
}

SkeletonMeshComponent::~SkeletonMeshComponent()
{
}

void SkeletonMeshComponent::Update(const float& deltaTime)
{
   
	componentDeltaTime = deltaTime;
	
}

void SkeletonMeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{
	dx::XMFLOAT3 tmpPos;
	dx::XMStoreFloat3(&tmpPos, GetOwner()->GetTransform().GetWorldPosition());

	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL) || !camera->CullAgainstAABB(boundingBoxes.GetAABB(), tmpPos))
	{
		
		
		renderer->DrawSkeleton(mesh, material, GetOwner()->GetTransform().GetWorldMatrix(), *camera, finalTransforms);
		RunAnimation(componentDeltaTime);

		
	}
}

void SkeletonMeshComponent::RunAnimation(const float& deltaTime)
{
	elapsedTime += deltaTime;
	if (elapsedTime >= 60.0f)
	{
		elapsedTime = 0.0f; //I just dont like the idea of it running to infinity.
	}
	if (currentAni == SkeletonStateMachine::IDLE)
	{	
		finalTransforms = skeletonAnimations[0].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[0].GetRootKeyJoints());
		
	}
	else if (currentAni == SkeletonStateMachine::WALK)
	{
		finalTransforms = skeletonAnimations[1].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[1].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::RUN|| currentAni == SkeletonStateMachine::UP)
	{
		finalTransforms = skeletonAnimations[2].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[2].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::ATTACK || currentAni == SkeletonStateMachine::DOWN)
	{
		finalTransforms = skeletonAnimations[3].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[3].GetRootKeyJoints());

		
	}
	else if (currentAni == SkeletonStateMachine::DEATH)
	{
		finalTransforms = skeletonAnimations[4].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[4].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::NONE)
	{
		//BlendAnimations(elapsedTime, 0.1f, 3, 2);
	}
	
}

void SkeletonMeshComponent::SetAnimationTrack(const SkeletonAni& skeletonAni, const SkeletonStateMachine& type)
{
	trackMap.insert({ type, skeletonAnimations.size() });

	skeletonAnimations.push_back(skeletonAni);

	
}

SkeletonAni& SkeletonMeshComponent::GetAnimationTrack(unsigned int trackNr)
{
	return skeletonAnimations[trackNr];
}

void SkeletonMeshComponent::Play(const SkeletonStateMachine& type)
{
	currentAni = type;


}

void SkeletonMeshComponent::BlendAnimations(float elapsedTime, float t, int a, int b)
{
	std::vector<dx::XMFLOAT4X4> ftA = skeletonAnimations[a].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[a].GetRootKeyJoints());
	std::vector<dx::XMFLOAT4X4> ftB = skeletonAnimations[b].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[b].GetRootKeyJoints());

	for (int bone = 0; bone < ftA.size(); bone++)
	{
		DirectX::SimpleMath::Matrix m = ftA[bone];
		DirectX::SimpleMath::Matrix m2 = ftB[bone];

		m.Decompose(s, r1, t1);
		m2.Decompose(s, r2, t2);

		translationM = m.CreateTranslation(t3.Lerp(t1, t2, t));
		rotationM = m.CreateFromQuaternion(r3.Slerp(r1, r2, t));

		finalTransforms[bone] = rotationM * translationM;

	}

}
