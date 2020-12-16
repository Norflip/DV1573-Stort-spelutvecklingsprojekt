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
#include <cmath>

enum class EnemyType
{
	BASE,
	CHARGER,
	NONE
};
enum class SkeletonStateMachine
{
	IDLE,
	WALK,
	RUN,
	ATTACK,
	NONE,
	DEATH,
	UP,
	DOWN,
	BLENDED,
	LOAD,
	UNLOAD,
	COMBINED,
};


ALIGN16
class SkeletonMeshComponent : public Component, public Resource
{
public:
	SkeletonMeshComponent(Mesh* mesh, Material* material, float timeScale = 1.0f);
	SkeletonMeshComponent(SkeletonMeshComponent* other);
	virtual ~SkeletonMeshComponent();

	Mesh* GetMesh() const { return this->mesh; }
	Material* GetMaterial() const { return this->material; }

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;
	
	void SetAnimationTrack(const SkeletonAni& skeletonAni, const SkeletonStateMachine& type);
	
	SkeletonAni& GetAnimationTrack(unsigned int trackNr);

	std::vector<dx::XMFLOAT4X4>& GetFinalTransforms() { return this->finalTransforms; }

	void SetTrack(const SkeletonStateMachine& type, bool playOnce);
	
	void PlayBlendAnimations(const float& deltaTime);
	
	void CreateCombinedAnimation(SkeletonStateMachine state1, SkeletonStateMachine state2, int startJoint, int endJoint);

	bool GetIsDone();
	void SetisDone(bool);
	bool& SetAndGetDoneDown();
	bool& SetAndGetDoneUp();
	float GetTimeScale() { return this->timeScale; }
	const bool GetDoneDeath() { return doneDeath; }
	void SetDoneDeath(bool doneDeath) { this->doneDeath = doneDeath; }
	std::vector<dx::XMFLOAT4X4> GetAnimationTransforms() { return this->finalTransforms; }
	//void SetAnimationTransforms(std::vector<dx::XMFLOAT4X4> transform);
	std::vector<SkeletonAni> GetAnimations() { return this->skeletonAnimations; }
	std::unordered_map<SkeletonStateMachine, unsigned int> GetTrackMap() { return this->trackMap; }
	SkeletonStateMachine GetCurrentAnimation() { return this->currentAni; }
	Bounds GetBounds() { return this->bounds; }
	void SetTimeScale(float time) { this->timeScale = time; }
	const EnemyType GetEnemyType() { return enemyType; }
	void SetEnemyType(const EnemyType& enemyType) { this->enemyType = enemyType; }

	//BLEND
	std::vector<dx::XMFLOAT4X4> GetBlendTransform() { return this->blendTransform; }
	void SetBlendTransform(std::vector<dx::XMFLOAT4X4> transform);
	void SetBlendedAnimTime(SkeletonStateMachine track1, SkeletonStateMachine track2);
	float GetBlendedAnimTime() { return this->blendAnimTime; }
	bool SetBlendedDown(bool trufal);
	bool SetBlendedUp(bool trufal);

	void SetBlendingTracksAndFactor(SkeletonStateMachine track1, SkeletonStateMachine track2, float factor, bool blend);

	ALIGN16_ALLOC;

private:
	float timeScale = 1.0f;
	Mesh* mesh;
	Material* material;
	std::vector<SkeletonAni> skeletonAnimations;
	float elapsedTime = 0.0f;

	std::unordered_map<SkeletonStateMachine, unsigned int> trackMap;

	std::vector<dx::XMFLOAT4X4> finalTransforms;
	std::vector<dx::XMFLOAT4X4> blendTransform;

	SkeletonStateMachine currentAni = SkeletonStateMachine::NONE;
	Bounds bounds;
	float componentDeltaTime = 0.0f;
	GameClock timer;
	bool playOnce = false;
	bool done = false;
	void PlayOnce(const float& deltaTime);
	void RunAnimation(const float& deltaTime);
	bool doneUp, doneDown;
	bool doneDeath = false;
	float count = 0.0f;
	EnemyType enemyType = EnemyType::NONE;

	//Blending
	SkeletonStateMachine track1;
	SkeletonStateMachine track2;
	float factor;
	bool blend;
	float blendAnimTime;
	bool blendedDown;
	bool blendedUp;
};

