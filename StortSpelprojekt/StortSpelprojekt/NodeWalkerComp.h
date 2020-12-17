#pragma once
#include "SkeletonMeshComponent.h"
#include "Path.h"
#include "Input.h"
#include "World.h"
#include "MetaProgress.h"
#include <algorithm>

// why are these global?
constexpr float HEIGHT = 0.6f; //height from ground
class PlayerComp;

namespace dx = DirectX;
class NodeWalkerComp :public Component
{
private:
	Path thePath;
	float speed;
	int currentNode;
	bool canWalk;
	float nodeRadius;

	bool isWalking;
	float length;
	dx::XMFLOAT3 moveVec;
	SkeletonMeshComponent* base;
	SkeletonMeshComponent* legs;
	RigidBodyComponent* rbComp;
	PlayerComp* playerComp;
	World* world;

	//Blending
	float finalFactor = 0.f;
	float factorValue = 0.f;
	float factorRange = 0.f;
	float animDeltaTime = 0.f;

	void StartWalking();
	void StopWalking();
public:
	//int currentNode;
	//bool canWalk;
	NodeWalkerComp();
	virtual ~NodeWalkerComp();
	void InitializePath(Path thePath);
	void InitAnimation();
	//void Reset(); 
	void Start();
	void Stop();
	void Update(const float& deltaTime);
	

	void SetPlayerComp(PlayerComp* playerComp) { this->playerComp = playerComp; }
	dx::XMFLOAT3 GetMoveVec();
	bool GetIsWalking() const { return isWalking; }
	void SetWorld(World* world) { this->world = world; }
	int GetCurrentNode()const { return this->currentNode; }
	float GetHouseProgress(); //returns how far the house has gone between 0 and 1 (0 is 0% and 1 is 100%) 
};

