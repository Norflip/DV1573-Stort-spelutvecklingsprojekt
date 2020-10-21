#include "SkeletonMeshComponent.h"

SkeletonMeshComponent::SkeletonMeshComponent(Mesh mesh, Material material) : mesh(mesh), material(material), boundingBoxes(mesh)
{
	boundingBoxes.CalcAABB();
	currentAni = SkeletonStateMachine::NONE;
	finalTransforms.resize(60);
	doneDown = false;
	doneUp = false;
	dx::XMMATRIX id = dx::XMMatrixIdentity();
	for (int bone = 0; bone < 60; bone++)
	{
		dx::XMStoreFloat4x4(&finalTransforms[bone], id);
	}
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
		if (playOnce)
		{
			PlayOnce();
		}
		else
		{
			RunAnimation(componentDeltaTime);
		}

		
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

void SkeletonMeshComponent::findChildren(SkeletonAni& track, unsigned int index)
{
	for (unsigned int i = 0; i < track.GetKeyFrames().size(); i++)
	{
		if (i == index)
		{
			continue;
		}

		if (track.GetKeyFrames()[i][0].parentName == track.GetKeyFrames()[index][0].name)
		{
			findChildren(track, i);
		}
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

void SkeletonMeshComponent::SetTrack(const SkeletonStateMachine& type, bool playOnce)
{
	currentAni = type;
	this->playOnce = playOnce;

}

void SkeletonMeshComponent::PlayOnce()
{

	
	timer.Start();
	timer.Update();
	float time = timer.GetSeconds();
	
	
	
	
	
	
	if (currentAni == SkeletonStateMachine::IDLE)
	{
		if (!done)
		{
			finalTransforms = skeletonAnimations[0].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[0].GetRootKeyJoints());


			if (fmodf(time * skeletonAnimations[0].GetFPS(), skeletonAnimations[0].GetAniLength()) >= 0)
			{
				done = true;
			}
		}

	}
	else if (currentAni == SkeletonStateMachine::WALK)
	{
		if (!done)
		{
			finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[1].GetRootKeyJoints());


			if (fmodf(time * skeletonAnimations[1].GetFPS(), skeletonAnimations[1].GetAniLength()) >= 0)
			{
				done = true;
				doneDown = true;
			}
		}
	}
	else if (currentAni == SkeletonStateMachine::RUN || currentAni == SkeletonStateMachine::UP)
	{
		
		if (!done)
		{
			finalTransforms = skeletonAnimations[2].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[2].GetRootKeyJoints());
			

			if (fmodf(time * skeletonAnimations[2].GetFPS(), skeletonAnimations[2].GetAniLength()) >= 0)
			{
				done = true;
				doneUp = true;
			}
		}
		
		
	}
	else if (currentAni == SkeletonStateMachine::ATTACK || currentAni == SkeletonStateMachine::DOWN)
	{
		if (!done)
		{
			finalTransforms = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[3].GetRootKeyJoints());


			if (fmodf(time * skeletonAnimations[3].GetFPS(), skeletonAnimations[3].GetAniLength()) >= 0)
			{
				done = true;
			}
		}


	}
	else if (currentAni == SkeletonStateMachine::DEATH)
	{
		if (!done)
		{
			finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), skeletonAnimations[4].GetRootKeyJoints());


			if (fmodf(time * skeletonAnimations[4].GetFPS(), skeletonAnimations[4].GetAniLength()) >= 0)
			{
				done = true;
			}
		}
	}

}

void SkeletonMeshComponent::BlendAnimations()
{
	//skapa ett nytt track.
	//ta bort alla ben ovanför/under rooten. alla keyframes och offsets. lägg till från andra tracket.
	std::map<std::string, unsigned int> map1;
	std::map<std::string, unsigned int> map2;

	map1 = skeletonAnimations[2].getBoneIDMap();

	map2 = skeletonAnimations[3].getBoneIDMap();

	
	for (unsigned int i = 0; i < skeletonAnimations[2].GetKeyFrames().size(); i++)
	{
		if (skeletonAnimations[2].GetKeyFrames()[i][0].parentName == skeletonAnimations[2].GetKeyFrames()[1][0].name)
		{
			findChildren(skeletonAnimations[2], i);
		}
	}


}

bool SkeletonMeshComponent::GetIsDone()
{
	return done;
}

void SkeletonMeshComponent::SetisDone(bool done)
{
	this->done = done;
}
