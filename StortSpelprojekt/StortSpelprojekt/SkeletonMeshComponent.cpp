#include "stdafx.h"
#include "SkeletonMeshComponent.h"

SkeletonMeshComponent::SkeletonMeshComponent(Mesh* mesh, Material* material, float timeScale) : mesh(mesh), material(material), bounds(),
timeScale(timeScale)
{
	bounds.CalculateAABB(mesh);
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
	if (GetOwner()->HasFlag(ObjectFlag::NO_CULL) || camera->InView(bounds, GetOwner()->GetTransform().GetWorldMatrix()))
	{		
		renderer->DrawSkeleton(mesh, material, GetOwner()->GetTransform().GetWorldMatrix(), finalTransforms);
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
		finalTransforms = skeletonAnimations[0].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), *skeletonAnimations[0].GetRootKeyJoints());
		
	}
	else if (currentAni == SkeletonStateMachine::WALK)
	{
		finalTransforms = skeletonAnimations[1].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::RUN|| currentAni == SkeletonStateMachine::UP)
	{
		finalTransforms = skeletonAnimations[2].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), *skeletonAnimations[2].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::ATTACK|| currentAni == SkeletonStateMachine::DOWN)
	{
		finalTransforms = skeletonAnimations[3].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), *skeletonAnimations[3].GetRootKeyJoints());

	}
	
	else if (currentAni == SkeletonStateMachine::DEATH)
	{
		finalTransforms = skeletonAnimations[4].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::BLENDED)
	{
		finalTransforms = skeletonAnimations[5].Makeglobal(elapsedTime, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
	}
	
}

void SkeletonMeshComponent::FindChildren(SkeletonAni& track, unsigned int& index, std::map<std::string, unsigned int>& map, std::string& name,
	std::string& secondName)
{
	for (unsigned int i = 0; i < track.GetKeyFrames().size(); i++)
	{
		
		std::string parentName = track.GetKeyFrames()[i][0].parentName;
		std::string childName = track.GetKeyFrames()[index][0].name;
		
		if (parentName == name|| parentName ==secondName)
		{
			continue;
		}
		
		if ( parentName==childName )
		{
			map.insert({ parentName, i-1 });
			FindChildren(track, i, map, name, secondName);
		}
		if (name == "spine01")
		{
			if (childName == "rightToe" || childName == "leftToe")
			{
				map.insert({ childName, index });
			}
		}
		else
		{
			

			if (childName == "leftPinky02" || childName == "leftMiddle02" || childName == "leftRing02" ||
				childName == "leftThumb02" || childName == "leftIndex02" || childName == "rightPinky02" || childName == "rightMiddle02" || childName == "rightRing02" ||
				childName == "rightThumb02" || childName == "rightIndex02" || childName == "head")
			{
				map.insert({ childName, index });
			}
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

	float time = 0.0f;
	
	
	
	
	
	if (currentAni == SkeletonStateMachine::IDLE)
	{
		if (!done)
		{
			timer.Restart();
			timer.Update();
			time = (float)timer.GetSeconds();
			time *= timeScale;

			float animationTime = time * skeletonAnimations[0].GetFPS();

			if (animationTime < skeletonAnimations[0].GetAniLength())
			{

				finalTransforms = skeletonAnimations[0].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[0].GetRootKeyJoints());
			}
			else
			{
				done = true;

			}
		}

	}
	else if (currentAni == SkeletonStateMachine::WALK)
	{
		if (!done)
		{
			timer.Restart();
			timer.Update();
			time = (float)timer.GetSeconds();
			time *= timeScale;

			float animationTime = time * skeletonAnimations[1].GetFPS();

			if (animationTime < skeletonAnimations[1].GetAniLength())
			{
				finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
			}
			else
			{
				done = true;

			}
		}
	}
	else if (currentAni == SkeletonStateMachine::RUN || currentAni == SkeletonStateMachine::UP)
	{
		
		if (!done)
		{
			
			timer.Start();
			timer.Update();
			time = (float)timer.GetSeconds();
			time *= timeScale;
			float animationTime = time * skeletonAnimations[3].GetFPS();

			if (animationTime < skeletonAnimations[3].GetAniLength())
			{
				
				finalTransforms = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[3].GetRootKeyJoints());
				
			}
			else
			{
				timer.Stop();
				done = true;
				doneUp = true;
			}
			
		}
		
		
	}
	else if (currentAni == SkeletonStateMachine::ATTACK || currentAni == SkeletonStateMachine::DOWN)
	{
		if (!done)
		{
			timer.Restart();
			timer.Update();
			time = (float)timer.GetSeconds();
			
			time *= timeScale;

			float animationTime = time * skeletonAnimations[3].GetFPS();

			if (animationTime < skeletonAnimations[3].GetAniLength())
			{
				
				finalTransforms = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[3].GetRootKeyJoints());
			}
			else
			{
				timer.Stop();
				done = true;
				doneDown = true;
			}
			
			
			
			
		}


	}
	else if (currentAni == SkeletonStateMachine::DEATH)
	{
	
		if (!done)
		{
			
			timer.Start();
			timer.Update();
			time = (float)timer.GetSeconds();
			time *= timeScale;
			float animationTime = time * skeletonAnimations[4].GetFPS();
			
			if (animationTime < skeletonAnimations[4].GetAniLength())
			{
				
				finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
			}
			else
			{
				done = true;
				timer.Stop();
			}
		}
	}

}

void SkeletonMeshComponent::BlendAnimations()
{

	//Need to have matching bones, need to know names, need to have the same fps and possibly same number of keys.

	//skapa ett nytt track.
	//ta bort alla ben ovanf�r/under rooten. alla keyframes och offsets. l�gg till fr�n andra tracket.
	std::map<std::string, unsigned int> map1;
	std::map<std::string, unsigned int> map2;

	//run animation
	for (unsigned int i = 0; i < skeletonAnimations[2].GetKeyFrames().size(); i++)
	{
		std::string name = skeletonAnimations[2].GetKeyFrames()[i][0].parentName;
		std::string errorName = "spine01";
		std::string errorName2 = "null";
		std::string childName = skeletonAnimations[2].GetKeyFrames()[0][0].name;
		if (name == errorName)
		{
			continue;
		}
		if (name == childName)
		{
			
			map1.insert({ name, i-1 });
			FindChildren(skeletonAnimations[2], i, map1, errorName, errorName2);

		}
		if (childName == "leftPinky02" || childName == "leftMiddle02" || childName == "leftRing02" ||
			childName == "leftThumb02" || childName == "leftIndex02" || childName == "rightPinky02" || childName == "rightMiddle02" || childName == "rightRing02" ||
			childName == "rightThumb02" || childName == "rightIndex02")
		{
			map2.insert({ childName, 0 });
		}
		
	}
	// attack animation
	for (unsigned int i = 0; i < skeletonAnimations[3].GetKeyFrames().size(); i++)
	{
		std::string errorName = "rightLeg";
		std::string errorName2 = "leftLeg";
		std::string name = skeletonAnimations[3].GetKeyFrames()[i][0].parentName;
		if (name == "rightLeg"||name == "leftLeg")
		{
			continue;
		}
		
		std::string childName = skeletonAnimations[3].GetKeyFrames()[0][0].name;
		if (name == childName)
		{
			if (name != "root")
			{
				map2.insert({ name, i-1 });
			}
			
			
			FindChildren(skeletonAnimations[3], i, map2, errorName, errorName2);
		}
		if (childName == "leftPinky02" || childName == "leftMiddle02" || childName == "leftRing02" ||
			childName == "leftThumb02" || childName == "leftIndex02" || childName == "rightPinky02" || childName == "rightMiddle02" || childName == "rightRing02" ||
			childName == "rightThumb02" || childName == "rightIndex02"||childName=="head")
		{
			map1.insert({ childName, 0 });
		}


		
	}
	
	
	SkeletonAni blended;

	std::vector<dx::SimpleMath::Matrix> offsets(41);
	std::vector<std::vector<Bone>> keyframes(41);
	for (std::pair<std::string, unsigned int> map : map1)
	{
		offsets[map.second] = skeletonAnimations[2].GetOffsets()[map.second];

		keyframes[map.second] = skeletonAnimations[2].GetKeyFrames()[map.second];

	}

	for (std::pair<std::string, unsigned int> map : map2)
	{
		offsets[map.second] = skeletonAnimations[3].GetOffsets()[map.second];

		keyframes[map.second] = skeletonAnimations[3].GetKeyFrames()[map.second];
	}

	blended.SetUpIDMapAndFrames(skeletonAnimations[2].GetBoneIDMap(), skeletonAnimations[2].GetFPS(), skeletonAnimations[2].GetAniLength());
	blended.SetOffsetsDirect(offsets);
	blended.SetKeyFramesDirect(keyframes);

	SetAnimationTrack(blended, SkeletonStateMachine::BLENDED);
}

bool SkeletonMeshComponent::GetIsDone()
{
	return done;
}

void SkeletonMeshComponent::SetisDone(bool done)
{
	this->done = done;
}

bool& SkeletonMeshComponent::SetAndGetDoneDown()
{
	return doneDown;
}

bool& SkeletonMeshComponent::SetAndGetDoneUp()
{
	return doneUp;
}
