#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "Bounds.h"
#include "Object.h"
#include "GameClock.h"
#include "Resource.h"

enum class SkeletonChargerStateMachine
{
	IDLE,
	WALK,
	LOAD,
	RUN,
	UNLOAD,
	ATTACK,
	DEATH,
	NONE,
};


ALIGN16
class SkeletonChargerMeshComponent : public Component, public Resource
{
public:
	SkeletonChargerMeshComponent(Mesh* mesh, Material* material, float timeScale = 1.0f);
	SkeletonChargerMeshComponent(SkeletonChargerMeshComponent* other);
	virtual ~SkeletonChargerMeshComponent();

	Mesh* GetMesh() const { return this->mesh; }
	Material* GetMaterial() const { return this->material; }

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;

	void SetAnimationTrack(const SkeletonAni& skeletonAni, const SkeletonChargerStateMachine& type);

	SkeletonAni& GetAnimationTrack(unsigned int trackNr);

	std::vector<dx::XMFLOAT4X4>& GetFinalTransforms() { return this->finalTransforms; }

	void SetTrack(const SkeletonChargerStateMachine& type, bool playOnce);
	void BlendAnimations();
	bool GetIsDone();
	void SetisDone(bool);
	bool& SetAndGetDoneDown();
	bool& SetAndGetDoneUp();
	float GetTimeScale() { return this->timeScale; }

	std::vector<dx::XMFLOAT4X4> GetAnimationTransforms() { return this->finalTransforms; }
	std::vector<SkeletonAni> GetAnimations() { return this->skeletonAnimations; }
	std::unordered_map<SkeletonChargerStateMachine, unsigned int> GetTrackMap() { return this->trackMap; }
	SkeletonChargerStateMachine GetCurrentAnimation() { return this->currentAni; }
	Bounds GetBounds() { return this->bounds; }
	void SetTimeScale(float time) { this->timeScale = time; }

	ALIGN16_ALLOC;

private:
	float timeScale = 1.0f;
	Mesh* mesh;
	Material* material;
	std::vector<SkeletonAni> skeletonAnimations;
	float elapsedTime = 0.0f;
	std::unordered_map<SkeletonChargerStateMachine, unsigned int> trackMap;
	std::vector<dx::XMFLOAT4X4> finalTransforms;
	SkeletonChargerStateMachine currentAni = SkeletonChargerStateMachine::NONE;
	Bounds bounds;
	float componentDeltaTime = 0.0f;
	GameClock timer;
	bool playOnce = false;
	bool done = false;
	void PlayOnce(const float& deltaTime);
	void RunAnimation(const float& deltaTime);
	void FindChildren(SkeletonAni& track, unsigned int& index, std::map<std::string, unsigned int>& map, std::string& name, std::string& secondName);
	bool doneUp, doneDown;
};

