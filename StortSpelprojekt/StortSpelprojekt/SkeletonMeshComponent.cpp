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
	doneDeath = false;
	dx::XMMATRIX id = dx::XMMatrixIdentity();
	for (int bone = 0; bone < 60; bone++)
	{
		dx::XMStoreFloat4x4(&finalTransforms[bone], id);
	}

}

SkeletonMeshComponent::SkeletonMeshComponent(SkeletonMeshComponent* other)
{
	this->mesh = other->GetMesh();
	this->material = other->GetMaterial();
	this->timeScale = other->GetTimeScale();

	for (size_t i = 0; i < other->GetAnimations().size(); i++)
	{
		skeletonAnimations.push_back(other->GetAnimations()[i]);
	}

	this->bounds = other->GetBounds();

	this->currentAni = other->GetCurrentAnimation();

	this->doneDown = false;
	this->doneUp = false;

	for (size_t bone = 0; bone < other->GetAnimationTransforms().size(); bone++)
	{
		dx::XMFLOAT4X4 matrix = other->GetAnimationTransforms()[bone];

		finalTransforms.push_back(matrix);
	}

	this->trackMap = other->GetTrackMap();

	float stop = 0;
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
	//if (GetOwner()->HasFlag(ObjectFlag::NO_CULL) || camera->InView(bounds, GetOwner()->GetTransform().GetWorldMatrix()))
	//{		
	renderer->DrawSkeleton(mesh, material, GetOwner()->GetTransform().GetWorldMatrix(), finalTransforms);
	if (playOnce)
	{
		PlayOnce(componentDeltaTime);
	}
	else
	{
		RunAnimation(componentDeltaTime);
	}
	//}
}

void SkeletonMeshComponent::RunAnimation(const float& deltaTime)
{
	elapsedTime += deltaTime;

	float time = elapsedTime;

	time *= timeScale;

	if (elapsedTime >= 60.0f)
	{
		elapsedTime = 0.0f; //I just dont like the idea of it running to infinity.
	}
	if (currentAni == SkeletonStateMachine::IDLE)
	{
		finalTransforms = skeletonAnimations[0].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[0].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::WALK)
	{
		finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::RUN || currentAni == SkeletonStateMachine::UP)
	{
		finalTransforms = skeletonAnimations[2].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[2].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::ATTACK || currentAni == SkeletonStateMachine::DOWN)
	{
		finalTransforms = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[3].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::DEATH)
	{
		finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::BLENDED)
	{
		finalTransforms = skeletonAnimations[5].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::LOAD)
	{
		finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::UNLOAD)
	{
		finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
	}

}

void SkeletonMeshComponent::FindChildren(SkeletonAni& track, unsigned int& index, std::map<std::string, unsigned int>& map, std::string& name,
	std::string& secondName)
{
	for (unsigned int i = 0; i < track.GetKeyFrames().size(); i++)
	{
		std::string parentName = track.GetKeyFrames()[i][0].parentName;
		std::string childName = track.GetKeyFrames()[index][0].name;

		if (parentName == name || parentName == secondName)
		{
			continue;
		}

		if (parentName == childName)
		{
			map.insert({ parentName, i - 1 });
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

void SkeletonMeshComponent::PlayOnce(const float& deltaTime)
{
	float time = 0.0f;
	bool doneOnce = false;
	

	if (currentAni == SkeletonStateMachine::IDLE)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			float animLength = skeletonAnimations[0].GetAniLength() / skeletonAnimations[0].GetFPS();

			if (time <= animLength)
			{
				//std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[0].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[0].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}

	else if (currentAni == SkeletonStateMachine::WALK)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			float animLength = skeletonAnimations[1].GetAniLength() / skeletonAnimations[1].GetFPS();

			if (time <= animLength)
			{
				//std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}

	else if (currentAni == SkeletonStateMachine::RUN)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			float animLength = skeletonAnimations[2].GetAniLength() / skeletonAnimations[2].GetFPS();

			if (time <= animLength)
			{
				//std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[2].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[2].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}

	else if (currentAni == SkeletonStateMachine::UP)
	{
		if (!doneOnce)
		{
			timer.Start();
			timer.Update();
			time = (float)timer.GetSeconds();
			time *= timeScale;
			float animationTime = time * skeletonAnimations[2].GetFPS();

			if (animationTime < skeletonAnimations[2].GetAniLength())
			{

				finalTransforms = skeletonAnimations[2].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[2].GetRootKeyJoints());

			}
			else
			{
				timer.Stop();
				doneOnce = true;
				doneUp = true;
			}
		}
	}

	else if (currentAni == SkeletonStateMachine::ATTACK)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			//Get the playtime for the animation in seconds.
			float animLength = skeletonAnimations[3].GetAniLength() / skeletonAnimations[3].GetFPS(); 

			if (time <= animLength)
			{
				//std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[3].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}

	else if (currentAni == SkeletonStateMachine::DOWN)
	{
		if (!doneOnce)
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
				doneOnce = true;
				doneDown = true;
			}
		}
	}
	else if (currentAni == SkeletonStateMachine::DEATH)
	{
		if (!doneDeath)
		{
			
			count += deltaTime;
			
			if (count < skeletonAnimations[trackMap[SkeletonStateMachine::DEATH]].GetAniLength() / skeletonAnimations[trackMap[SkeletonStateMachine::DEATH]].GetFPS())
			{

				finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::DEATH]].Makeglobal(count, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::DEATH]].GetRootKeyJoints());
				
			}
			else
			{
				timer.Stop();
				doneOnce = true;
				doneDeath = true;
				count = 0.0f;
			}
		}
		
	}
	else if (currentAni == SkeletonStateMachine::LOAD)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			float animLength = skeletonAnimations[1].GetAniLength() / skeletonAnimations[1].GetFPS();

			if (time <= animLength)
			{
				//std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}
	else if (currentAni == SkeletonStateMachine::UNLOAD)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			float animLength = skeletonAnimations[4].GetAniLength() / skeletonAnimations[4].GetFPS();

			if (time <= animLength)
			{
				//std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
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

			map1.insert({ name, i - 1 });
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
		if (name == "rightLeg" || name == "leftLeg")
		{
			continue;
		}

		std::string childName = skeletonAnimations[3].GetKeyFrames()[0][0].name;
		if (name == childName)
		{
			if (name != "root")
			{
				map2.insert({ name, i - 1 });
			}


			FindChildren(skeletonAnimations[3], i, map2, errorName, errorName2);
		}
		if (childName == "leftPinky02" || childName == "leftMiddle02" || childName == "leftRing02" ||
			childName == "leftThumb02" || childName == "leftIndex02" || childName == "rightPinky02" || childName == "rightMiddle02" || childName == "rightRing02" ||
			childName == "rightThumb02" || childName == "rightIndex02" || childName == "head")
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
