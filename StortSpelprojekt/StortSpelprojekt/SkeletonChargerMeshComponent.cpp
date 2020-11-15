#include "stdafx.h"
#include "SkeletonChargerMeshComponent.h"

SkeletonChargerMeshComponent::SkeletonChargerMeshComponent(Mesh* mesh, Material* material, float timeScale) : mesh(mesh), material(material), bounds(), timeScale(timeScale)
{
	bounds.CalculateAABB(mesh);
	currentAni = SkeletonChargerStateMachine::NONE;
	finalTransforms.resize(60);
	doneDown = false;
	doneUp = false;
	dx::XMMATRIX id = dx::XMMatrixIdentity();

	for (int bone = 0; bone < 60; bone++)
	{
		dx::XMStoreFloat4x4(&finalTransforms[bone], id);
	}
}

SkeletonChargerMeshComponent::SkeletonChargerMeshComponent(SkeletonChargerMeshComponent* other)
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

SkeletonChargerMeshComponent::~SkeletonChargerMeshComponent()
{
}

void SkeletonChargerMeshComponent::Update(const float& deltaTime)
{
	componentDeltaTime = deltaTime;
}

void SkeletonChargerMeshComponent::Draw(Renderer* renderer, CameraComponent* camera)
{	
	renderer->DrawSkeleton(mesh, material, GetOwner()->GetTransform().GetWorldMatrix(), finalTransforms);
	if (playOnce)
	{
		PlayOnce(componentDeltaTime);
	}
	else
	{
		RunAnimation(componentDeltaTime);
	}
}

void SkeletonChargerMeshComponent::SetAnimationTrack(const SkeletonAni& skeletonAni, const SkeletonChargerStateMachine& type)
{
	trackMap.insert({ type, skeletonAnimations.size() });

	skeletonAnimations.push_back(skeletonAni);
}

SkeletonAni& SkeletonChargerMeshComponent::GetAnimationTrack(unsigned int trackNr)
{
	return skeletonAnimations[trackNr];
}

void SkeletonChargerMeshComponent::SetTrack(const SkeletonChargerStateMachine& type, bool playOnce)
{
	currentAni = type;
	this->playOnce = playOnce;
}

bool SkeletonChargerMeshComponent::GetIsDone()
{
	return false;
}

void SkeletonChargerMeshComponent::PlayOnce(const float& deltaTime)
{
	float time = 0.0f;
	bool doneOnce = false;

	if (currentAni == SkeletonChargerStateMachine::IDLE)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			float animLength = skeletonAnimations[0].GetAniLength() / skeletonAnimations[0].GetFPS();

			if (time <= animLength)
			{
				std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[0].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[0].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}
	else if (currentAni == SkeletonChargerStateMachine::LOAD)
	{
		if (!doneOnce)
		{
			elapsedTime += deltaTime;
			time = elapsedTime;
			time *= timeScale;

			float animLength = skeletonAnimations[1].GetAniLength() / skeletonAnimations[1].GetFPS();

			if (time <= animLength)
			{
				std::cout << time << std::endl;
				finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
			}
			else
			{
				elapsedTime = 0.0f;
				doneOnce = true;
			}
		}
	}
}

void SkeletonChargerMeshComponent::RunAnimation(const float& deltaTime)
{
	elapsedTime += deltaTime;
	float time = elapsedTime;
	time *= timeScale;

	if (elapsedTime >= 60.0f)
	{
		elapsedTime = 0.0f; 
	}
	if (currentAni == SkeletonChargerStateMachine::IDLE)
	{
		finalTransforms = skeletonAnimations[0].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[0].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonChargerStateMachine::LOAD)
	{
		finalTransforms = skeletonAnimations[1].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[1].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonChargerStateMachine::RUN)
	{
		finalTransforms = skeletonAnimations[2].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[2].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonChargerStateMachine::ATTACK)
	{
		finalTransforms = skeletonAnimations[3].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[3].GetRootKeyJoints());
	}
	else if (currentAni == SkeletonChargerStateMachine::UNLOAD)
	{
		finalTransforms = skeletonAnimations[4].Makeglobal(time, dx::XMMatrixIdentity(), *skeletonAnimations[4].GetRootKeyJoints());
	}
}

void SkeletonChargerMeshComponent::FindChildren(SkeletonAni& track, unsigned int& index, std::map<std::string, unsigned int>& map, std::string& name, std::string& secondName)
{
}
