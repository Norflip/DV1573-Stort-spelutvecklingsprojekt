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
	this->factor = 0;
	this->blend = false;

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
	else if (!playOnce)
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

	if(blend)
		PlayBlendAnimations(time);

	if (elapsedTime >= 60.0f)
	{
		elapsedTime = 0.0f; //I just dont like the idea of it running to infinity.
	}
	if (currentAni == SkeletonStateMachine::IDLE)
	{
		finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::IDLE]].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::IDLE]].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::WALK)
	{
		finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::WALK]].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::WALK]].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::RUN || currentAni == SkeletonStateMachine::UP)
	{
		finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetRootKeyJoints());
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
		finalTransforms = GetBlendTransform();
	}
	else if (currentAni == SkeletonStateMachine::LOAD)
	{
		finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::UNLOAD)
	{
		finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonStateMachine::COMBINED)
	{
		finalTransforms = skeletonAnimations[6].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[6].GetRootKeyJoints());
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

void SkeletonMeshComponent::CreateCombinedAnimation(SkeletonStateMachine state1, SkeletonStateMachine state2, int startJoint, int endJoint)
{
	int anim1 = trackMap[state1];
	int anim2 = trackMap[state2];

	SkeletonAni combinedAnim;

	std::map<std::string, unsigned int> map1;

	std::vector<dx::SimpleMath::Matrix> offSets(skeletonAnimations[anim2].GetOffsets().size());
	std::vector<std::vector<Bone>> animKeyFrames(skeletonAnimations[anim2].GetKeyFrames().size());

	skeletonAnimations[anim2].FindChildren(componentDeltaTime, dx::XMMatrixIdentity(), *skeletonAnimations[anim2].GetRootKeyJoints(), map1, startJoint, endJoint);

	for (int i = 0; i < animKeyFrames.size(); i++)
	{
		animKeyFrames[i] = skeletonAnimations[anim1].GetKeyFrames()[i];
	}

	for (int i = 0; i < offSets.size(); i++)
	{
		offSets[i] = skeletonAnimations[anim1].GetOffsets()[i];
	}

	for (std::pair<std::string, unsigned int> map : map1)
	{
		offSets[map.second] = skeletonAnimations[anim2].GetOffsets()[map.second];
		animKeyFrames[map.second] = skeletonAnimations[anim2].GetKeyFrames()[map.second];
	}

	//Set the bones that are going to be affected, the speed of the animation and the length.
	combinedAnim.SetUpIDMapAndFrames(skeletonAnimations[anim1].GetBoneIDMap(), skeletonAnimations[anim1].GetFPS(), skeletonAnimations[anim1].GetAniLength());
	combinedAnim.SetOffsetsDirect(offSets); //Set the offsets
	combinedAnim.SetKeyFramesDirect(animKeyFrames);
	SetAnimationTrack(combinedAnim, SkeletonStateMachine::COMBINED);
}

void SkeletonMeshComponent::PlayOnce(const float& deltaTime)
{
	float time = 0.0f;
	bool doneOnce = false;
	doneDeath = false;

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
		if (!doneUp)
		{
			/*timer.Start();
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
			}*/
			count += deltaTime;

			if (count < skeletonAnimations[trackMap[SkeletonStateMachine::UP]].GetAniLength() / skeletonAnimations[trackMap[SkeletonStateMachine::UP]].GetFPS())
			{

				finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::UP]].Makeglobal(count, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::UP]].GetRootKeyJoints());

			}
			else
			{
				doneUp = true;
				count = 0.0f;
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
		if (!doneDown)
		{
			/*timer.Start();
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
			}*/
			count += deltaTime;

			if (count < skeletonAnimations[trackMap[SkeletonStateMachine::DOWN]].GetAniLength() / skeletonAnimations[trackMap[SkeletonStateMachine::DOWN]].GetFPS())
			{

				finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::DOWN]].Makeglobal(count, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::DOWN]].GetRootKeyJoints());

			}
			else
			{
				doneDown = true;
				count = 0.0f;
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

	else if (currentAni == SkeletonStateMachine::BLENDED)
	{
	if (!doneOnce)
	{
		elapsedTime += deltaTime;
		time = elapsedTime;
		time *= timeScale;

		//Get the playtime for the animation in seconds.
		float animLength = GetBlendedAnimTime();

		if (time <= animLength)
		{
			//std::cout << time << std::endl;
			finalTransforms = GetBlendTransform();
		}
		else
		{
			elapsedTime = 0.0f;
			if (blendedDown)
			{
				doneDown = true;
			}
			else
			{
				doneOnce = true;
			}
			
		}
	}
	}

	else if (currentAni == SkeletonStateMachine::COMBINED)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			//Get the playtime for the animation in seconds.
			float animLength = skeletonAnimations[5].GetAniLength() / skeletonAnimations[5].GetFPS();

			if (time <= animLength)
			{
				//std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[5].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[5].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}
}

void SkeletonMeshComponent::PlayBlendAnimations(const float& deltaTime)
{
	std::vector<dx::XMFLOAT4X4> anim1 = skeletonAnimations[trackMap[this->track1]].Makeglobal(deltaTime, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[this->track1]].GetRootKeyJoints());
	std::vector<dx::XMFLOAT4X4> anim2 = skeletonAnimations[trackMap[this->track2]].Makeglobal(deltaTime, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[this->track2]].GetRootKeyJoints());

	std::vector <dx::XMMATRIX> blendMat1;
	std::vector <dx::XMMATRIX> blendMat2;
	std::vector <dx::XMMATRIX> blendCalcMat;
	std::vector<dx::XMFLOAT4X4> blendedAnim;
	dx::XMFLOAT4X4 storeFloat;

	for (int i = 0; i < anim1.size(); i++)
	{
		blendMat1.push_back(dx::XMLoadFloat4x4(&anim1[i]));
		blendMat2.push_back(dx::XMLoadFloat4x4(&anim2[i]));
		blendCalcMat.push_back(blendMat1[i] * (1 - this->factor) + blendMat2[i] * this->factor);

		dx::XMStoreFloat4x4(&storeFloat, blendCalcMat[i]);

		blendedAnim.push_back(storeFloat);
	}

	SetBlendTransform(blendedAnim);
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

void SkeletonMeshComponent::SetBlendTransform(std::vector<dx::XMFLOAT4X4> transform)
{
	this->blendTransform = transform;
}

void SkeletonMeshComponent::SetBlendingTracksAndFactor(SkeletonStateMachine track1, SkeletonStateMachine track2, float factor, bool blend)
{
	this->track1 = track1;
	this->track2 = track2;
	this->factor = factor;
	this->blend = blend;
}

void SkeletonMeshComponent::SetBlendedAnimTime(SkeletonStateMachine track1, SkeletonStateMachine track2)
{
	float anim1Time = skeletonAnimations[trackMap[track1]].GetAniLength() / skeletonAnimations[trackMap[track1]].GetFPS();
	float anim2Time = skeletonAnimations[trackMap[track2]].GetAniLength() / skeletonAnimations[trackMap[track2]].GetFPS();
	this->blendedAnimTime = anim1Time * (1 - this->factor) + anim2Time * this->factor;
}

bool SkeletonMeshComponent::SetBlendedDown(bool trufal)
{
	this->blendedDown = trufal;
	return this->blendedDown;
}
