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
		finalTransforms = skeletonAnimations[6].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[6].GetRootKeyJoints());
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
		finalTransforms = skeletonAnimations[5].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[5].GetRootKeyJoints());
	}

}

void SkeletonMeshComponent::FindChildren(SkeletonAni& track, int index, std::map<std::string, unsigned int>& map, std::string& name,
	std::string& secondName)
{
	//EMILS HARDCODED STUFF
	/*
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
			std::cout << "Parent Name: " << parentName << std::endl;
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
			std::cout << "Child Name: " << childName << std::endl;
			if (childName == "leftPinky02" || childName == "leftMiddle02" || childName == "leftRing02" ||
				childName == "leftThumb02" || childName == "leftIndex02" || childName == "rightPinky02" || childName == "rightMiddle02" || childName == "rightRing02" ||
				childName == "rightThumb02" || childName == "rightIndex02" || childName == "head")
			{
				map.insert({ childName, index });
				
			}
		}
	}
	*/
	
	//std::string parentName;
	//std::string childName;
	//childName = track.GetKeyFrames()[index][0].name;

	//VIKTOR

	// GÖR ATT MAN FÅR HÅRDKODA SLUTNODEN OCKSÅ
	//for (unsigned int i = 0; i < 4; i++)
	//{
	//	parentName = track.GetKeyFrames()[i][0].parentName;
	//	childName = track.GetKeyFrames()[index][0].name;
	//	
	//	if (parentName == name)
	//	{
	//		continue;
	//	}

	//	if (parentName == childName)
	//	{
	//		//map.insert({ parentName, i - 1 });
	//		//FindChildren(track, i, map, name, secondName);
	//		
	//	}
	//	std::cout << "Parent Name: " << childName << std::endl;
	//}

	//std::cout << "Child Name: " << childName << std::endl;

	//track.GetBones().size()
	//for (unsigned int i = 0; i < track.GetBones().size(); i++) //recursively find all the children and repeat.
	//{
	//	
	//	parentName = track.GetKeyFrames()[i][0].parentName;
	//	childName = track.GetKeyFrames()[index][0].name;

	//	if (track.GetkeyBones()[i][0].parentName == track.GetRootKeyJoints()[0][0].name)
	//	{
	//		FindChildren(track, i, map, name, secondName);
	//		//FindChildren(componentDeltaTime, toRoot, keyBones[i]);
	//		std::cout << "Parent Name: " << childName << std::endl;

	//	}
	//}

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

	//LITE PROGRESS
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

void SkeletonMeshComponent::CreateBlendedAnimation()
{
	/*Har man två animationer A1 och A2 för ett skelett,
	och en faktor f som går mellan [0-1] och på så vis avgör övergången från A1 till A2.
	Om f=0, så är det 100% animation A1 som spelas, om f=0.4 så är det 40% A1 och 60% A2 osv.

	Slutgiltiga animationen mellan dessa blir en kombination av båda (simplifierat):
	A = A1 * (f - 1) + A2 * f
	*/
	//DO DIS

	SkeletonAni blendedAnim;

	blendedAnim = skeletonAnimations[0];

	SetAnimationTrack(blendedAnim, SkeletonStateMachine::BLENDED);
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
		float animLength = skeletonAnimations[6].GetAniLength() / skeletonAnimations[6].GetFPS();

		if (time <= animLength)
		{
			//std::cout << time << std::endl;
			finalTransforms = skeletonAnimations[6].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[6].GetRootKeyJoints());
		}
		else
		{
			elapsedTime = 0.0f;
			doneOnce = true;
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

void SkeletonMeshComponent::PlayBlendAnimations(SkeletonStateMachine state1, SkeletonStateMachine state2, float factor)
{
	std::vector<dx::SimpleMath::Matrix> offSets(skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetOffsets().size());
	std::vector<std::vector<Bone>> animKeyFrames(skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetKeyFrames().size());

	//A = A1 * (f - 1) + A2 * f

		///STREATEGI FÖR DAGEN
		/// HÄMTA ALLA VARIABLER FRÅN BONE STRUCKTEN FÖR SIG OCH FÖRSÖK BLENDA MELLAN DEM
		/// //GÖR EN FUNKTION SOM TAR IN TVÅ STRUCTS

		//dx::SimpleMath::Quaternion quat1;
		//dx::SimpleMath::Quaternion quat2;
		//dx::SimpleMath::Quaternion quatFinal;

		//dx::SimpleMath::Vector3 trans1;
		//dx::SimpleMath::Vector3 trans2;
		//dx::SimpleMath::Vector3 transFinal;

		//blendFactor = 1.0f;
		//
		////ROTATION QUAT VERKAR INTE VARA KORREKT
		//for (int i = 0; i < animKeyFrames.size(); i++)
		//{

		//	//animKeyFrames[i][0].rotationQuaternion = skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetKeyFrames()[i][0].rotationQuaternion * (1 - blendFactor) + skeletonAnimations[trackMap[SkeletonStateMachine::ATTACK]].GetKeyFrames()[i][0].rotationQuaternion * blendFactor;
		//	quat1 = skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetKeyFrames()[i][0].rotationQuaternion;
		//	quat2 = skeletonAnimations[trackMap[SkeletonStateMachine::ATTACK]].GetKeyFrames()[i][0].rotationQuaternion;

		//	quatFinal = /*quat1 * (1 - blendFactor) + quat2 * blendFactor;*/ quatFinal.Slerp(quat1, quat2, blendFactor);

		//	trans1 = skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetKeyFrames()[i][0].translationVector;
		//	trans2 = skeletonAnimations[trackMap[SkeletonStateMachine::ATTACK]].GetKeyFrames()[i][0].translationVector;
		//	transFinal = trans1 * (1 - blendFactor) + trans2 * blendFactor;
		//}

		/*for (int i = 0; i < offSets.size(); i++)
		{
			offSets[i] = skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetOffsets()[i];
		}*/

	skeletonAnimations[trackMap[SkeletonStateMachine::BLENDED]].MergeKeys(
		skeletonAnimations[trackMap[state1]].GetKeyFrames(),
		skeletonAnimations[trackMap[state2]].GetKeyFrames(),
		factor);


	float animLength = skeletonAnimations[trackMap[state1]].GetAniLength() * (1 - factor) + skeletonAnimations[trackMap[state2]].GetAniLength() * factor;
	float fps = skeletonAnimations[trackMap[state1]].GetFPS() * (1 - factor) + skeletonAnimations[trackMap[state2]].GetFPS() * factor;

	//skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].SetQuaternionsDirect(quatFinal);
	//skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].SetTransVector(transFinal);

	skeletonAnimations[trackMap[SkeletonStateMachine::BLENDED]].SetUpIDMapAndFrames(skeletonAnimations[trackMap[state1]].GetBoneIDMap(), fps, animLength);
	//skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].SetOffsetsDirect(offSets); //Set the offsets
	//skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].SetKeyFramesDirect(animKeyFrames);

	finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::BLENDED]].Makeglobal(componentDeltaTime, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::BLENDED]].GetRootKeyJoints());
	//lerp(diff, diff2, blendFactor);

	//TESTA ATT LERPA SOM FOGGEN I FOG PIXELSHADERN
	//finalTransforms = skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[trackMap[SkeletonStateMachine::RUN]].GetRootKeyJoints());

	//testTransform = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[3].GetRootKeyJoints());

}

void SkeletonMeshComponent::BlendAnimations()
{



	//Need to have matching bones, need to know names, need to have the same fps and possibly same number of keys.

	//skapa ett nytt track.
	//ta bort alla ben ovanf�r/under rooten. alla keyframes och offsets. l�gg till fr�n andra tracket.
	std::map<std::string, unsigned int> map1;
	std::map<std::string, unsigned int> map2;

	//EMILS HARDCODED BLEND
	/*
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
	 */

	//VIKTOR
	for (unsigned int i = 0; i < skeletonAnimations[2].GetKeyFrames().size(); i++)
	{
		std::string name = skeletonAnimations[2].GetKeyFrames()[i][0].parentName;
		std::string errorName = "spine01";
		std::string errorName2 = "null";
		std::string childName = skeletonAnimations[2].GetKeyFrames()[0][0].name;

		if (name == childName)
		{

			map1.insert({ name, i - 1 });
			//FindChildren(skeletonAnimations[2], i, map1, errorName, errorName2);

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

		std::string childName = skeletonAnimations[3].GetKeyFrames()[0][0].name;
		if (name == childName)
		{
			if (name != "root")
			{
				map2.insert({ name, i - 1 });
			}


			//FindChildren(skeletonAnimations[3], i, map2, errorName, errorName2);
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

	//SetAnimationTrack(blended, SkeletonStateMachine::BLENDED);
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

//void SkeletonMeshComponent::SetAnimationTransforms(std::vector<dx::XMFLOAT4X4> transform)
//{
//	this->finalTransforms = transform;
//}
