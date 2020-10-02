#include "SkeletonMeshComponent.h"

SkeletonMeshComponent::SkeletonMeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material), boundingBoxes(mesh)
{
	boundingBoxes.CalcAABB();
	currentAni = StateMachine::IDLE; // need to figure out where to edit this.
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
		RunAnimation(0.0f);

		skeletonAnimations[trackMap[currentAni]].getBoneIDMap();
	}
}

void SkeletonMeshComponent::RunAnimation(const float& deltaTime)
{
	elapsedTime += deltaTime;
	if (elapsedTime >= 60.0f)
	{
		elapsedTime = 0.0f; //I just dont like the idea of it running to infinity.
	}
	if (currentAni != StateMachine::NONE)
	{	//I AM USING MAP HERE THIS MIGHT CREATE COPIES!!!
		skeletonAnimations[trackMap[currentAni]].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), skeletonAnimations[trackMap[currentAni]].GetRootKeyJoints());
		//I AM COPYING HERE I DONT KNOW HOW ELSE TO DO IT!!
		finalTransforms = skeletonAnimations[trackMap[currentAni]].bones;
	}
	//Need to add functionality for multiple flags in one.
}

void SkeletonMeshComponent::SetAnimationTrack(const SkeletonAni& skeletonAni, const StateMachine& type)
{
	trackMap.insert({ type, skeletonAnimations.size() });

	skeletonAnimations.push_back(skeletonAni);

	
}

SkeletonAni& SkeletonMeshComponent::GetAnimationTrack(unsigned int trackNr)
{
	return skeletonAnimations[trackNr];
}

void SkeletonMeshComponent::play(const StateMachine& type)
{
	currentAni = type;
}
