#pragma once
#include "SkeletonMeshComponent.h"
#include "Path.h"
#include "Input.h"
#include "World.h"

// why are these global?
constexpr float HEIGHT = 0.6f; //height from ground
class PlayerComp;

namespace dx = DirectX;
class NodeWalkerComp :public Component
{
private:
	//std::vector<Node> nodes;
	Path thePath;
	float speed;
	//int currentNode;
	int nextChosen;
	float nodeRadius;

	bool isWalking;
	float length;
	//dx::XMFLOAT3 lastPos;
	dx::XMFLOAT3 moveVec;
	SkeletonMeshComponent* base;
	SkeletonMeshComponent* legs;
	RigidBodyComponent* rbComp;
	PlayerComp* playerComp;
	dx::XMFLOAT3 pos3;
	World* world;

	void StartAnim();
	void StopAnim();
public:
	int currentNode;
	bool canWalk;
	NodeWalkerComp();
	virtual ~NodeWalkerComp();
	void InitializePath(Path thePath);
	void InitAnimation();
	void Reset(); //is public cus is meant to be called outside of class if needed
	void Start();
	void Stop();
	void GetPlayerInfo(PlayerComp* playerComp);
	void Update(const float& deltaTime);
	//dx::XMFLOAT3 GetLastPos();
	dx::XMFLOAT3 GetMoveVec();
	bool GetIsWalking() const { return isWalking; };
	void SetWorld(World* world) { this->world = world; }
};

