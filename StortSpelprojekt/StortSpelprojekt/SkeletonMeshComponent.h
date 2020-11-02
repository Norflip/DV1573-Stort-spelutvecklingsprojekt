#pragma once
#include "Component.h"
#include "Mesh.h"
#include "Material.h"
#include "Transform.h"
#include "CameraComponent.h"
#include "BoundingBoxes.h"
#include "Object.h"
#include "GameClock.h"

enum SkeletonStateMachine
{
	IDLE,
	WALK,
	RUN,
	ATTACK,
	NONE,
	DEATH,
	UP,
	DOWN,
	BLENDED
};


ALIGN16
class SkeletonMeshComponent : public Component
{
public:
	SkeletonMeshComponent(Mesh* mesh, Material* material);
	virtual ~SkeletonMeshComponent();

	Mesh* GetMesh() const { return this->mesh; }
	Material* GetMaterial() const { return this->material; }

	void Update(const float& deltaTime) override;
	void Draw(Renderer* renderer, CameraComponent* camera) override;
	
	void SetAnimationTrack(const SkeletonAni& skeletonAni, const SkeletonStateMachine& type);
	
	SkeletonAni& GetAnimationTrack(unsigned int trackNr);

	void SetTrack(const SkeletonStateMachine& type, bool playOnce);
	
	void BlendAnimations();
	bool GetIsDone();
	void SetisDone(bool);
	bool& SetAndGetDoneDown();
	bool& SetAndGetDoneUp();

	ALIGN16_ALLOC;

private:
	Mesh* mesh;
	Material* material;
	std::vector<SkeletonAni> skeletonAnimations;
	float elapsedTime = 0.0f;
	std::unordered_map<SkeletonStateMachine, unsigned int> trackMap;
	std::vector<dx::XMFLOAT4X4> finalTransforms;
	SkeletonStateMachine currentAni = SkeletonStateMachine::NONE;
	BoundingBox bounds;
	float componentDeltaTime = 0.0f;
	GameClock timer;
	bool playOnce = false;
	bool done = false;
	void PlayOnce();
	void RunAnimation(const float& deltaTime);
	void FindChildren(SkeletonAni& track, unsigned int& index, std::map<std::string, unsigned int>& map, std::string& name, std::string& secondName);
	bool doneUp, doneDown;
};

