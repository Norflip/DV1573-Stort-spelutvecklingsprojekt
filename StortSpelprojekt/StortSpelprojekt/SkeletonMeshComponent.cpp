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

void SkeletonMeshComponent::PlayOnce(const SkeletonStateMachine& type)
{
	timer.Restart();
	float time = (float)timer.GetSeconds();

	

	if (type == SkeletonStateMachine::IDLE)
	{
		float animationTime = time * skeletonAnimations[0].GetFPS();

		float currentFrame = fmodf(animationTime, skeletonAnimations[0].GetAniLength());

		if (currentFrame < skeletonAnimations[0].GetAniLength())
		{
			finalTransforms = skeletonAnimations[0].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[0].GetRootKeyJoints());
		}
		

	}
	else if (type == SkeletonStateMachine::WALK)
	{
		float animationTime = time * skeletonAnimations[1].GetFPS();

		float currentFrame = fmodf(animationTime, skeletonAnimations[1].GetAniLength());
		if (currentFrame < skeletonAnimations[1].GetAniLength())
		{
			finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[1].GetRootKeyJoints());
		}
		
	}
	else if (type == SkeletonStateMachine::RUN || currentAni == SkeletonStateMachine::UP)
	{

		float animationTime = time * skeletonAnimations[2].GetFPS();

		float currentFrame = fmodf(animationTime, skeletonAnimations[2].GetAniLength());
		if (currentFrame < skeletonAnimations[2].GetAniLength())
		{
			finalTransforms = skeletonAnimations[2].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[2].GetRootKeyJoints());
		}
		
	}
	else if (type == SkeletonStateMachine::ATTACK || currentAni == SkeletonStateMachine::DOWN)
	{

		float animationTime = time * skeletonAnimations[3].GetFPS();

		float currentFrame = fmodf(animationTime, skeletonAnimations[3].GetAniLength());
		if (currentFrame < skeletonAnimations[3].GetAniLength())
		{
			finalTransforms = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[3].GetRootKeyJoints());
		}
		


	}
	else if (type == SkeletonStateMachine::DEATH)
	{
		float animationTime = time * skeletonAnimations[4].GetFPS();

		float currentFrame = fmodf(animationTime, skeletonAnimations[4].GetAniLength());
		if (currentFrame < skeletonAnimations[4].GetAniLength())
		{
			finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[4].GetRootKeyJoints());
		}
		
	}
}

void SkeletonMeshComponent::BlendAnimations(float elapsedTime, float t, int a, int b)
{
	//skapa ett nytt track.
	//ta bort alla ben ovanför/under rooten. alla keyframes och offsets. lägg till från andra tracket.

}
