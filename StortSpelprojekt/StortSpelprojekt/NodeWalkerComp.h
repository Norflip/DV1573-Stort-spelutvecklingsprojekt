#pragma once
#include "SkeletonMeshComponent.h"
#include "Path.h"
#include "Input.h"
// why are these global?
constexpr float HEIGHT = 1.f; //height from ground
const float OFFSET = CHUNK_SIZE / 2.0f;

namespace dx = DirectX;
class NodeWalkerComp :public Component
{
private:
	//std::vector<Node> nodes;
	Path thePath;
	float speed;
	int currentNode;
	int nextChosen;
	float nodeRadius;
	bool canWalk;
	bool isWalking;
	float length;
	dx::XMFLOAT3 lastPos;
	SkeletonMeshComponent* base;
	SkeletonMeshComponent* legs;
	RigidBodyComponent* rbComp;

	void StartAnim();
	void StopAnim();
public:
	NodeWalkerComp();
	virtual ~NodeWalkerComp();
	void InitializePath(Path thePath);
	void InitAnimation();
	void Reset(); //is public cus is meant to be called outside of class if needed
	void Start();
	void Stop();

	void Update(const float& deltaTime);
	dx::XMFLOAT3 GetLastPos();
	bool GetIsWalking() const { return isWalking; };

	//void SetPosition(dx::XMVECTOR pos);

};

