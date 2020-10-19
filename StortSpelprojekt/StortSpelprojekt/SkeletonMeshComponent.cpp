#include "SkeletonMeshComponent.h"

SkeletonMeshComponent::SkeletonMeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material), boundingBoxes(mesh)
{
	boundingBoxes.CalcAABB();
	currentAni = SkeletonStateMachine::IDLE;
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
	else if (currentAni == SkeletonStateMachine::RUN)
	{
		finalTransforms = skeletonAnimations[2].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[2].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::ATTACK)
	{
		finalTransforms = skeletonAnimations[3].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[3].GetRootKeyJoints());

		
	}
	else if (currentAni == SkeletonStateMachine::DEATH)
	{
		finalTransforms = skeletonAnimations[4].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[4].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::NONE)
	{
		/*std::vector<dx::XMFLOAT4X4> ftA = skeletonAnimations[4].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[2].GetRootKeyJoints());
		std::vector<dx::XMFLOAT4X4> ftB = skeletonAnimations[4].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[3].GetRootKeyJoints());
		std::vector<dx::SimpleMath::Matrix> A(60);
		std::vector<dx::SimpleMath::Matrix> B(60);

		for (int i = 0; i < ftA.size(); i++)
		{
			A.push_back(ftA[i]);
			B.push_back(ftB[i]);

			A[i].Lerp(A[i], B[i], 0.5f, A[i]);

			finalTransforms.push_back(A[i]);
		}*/
		
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
