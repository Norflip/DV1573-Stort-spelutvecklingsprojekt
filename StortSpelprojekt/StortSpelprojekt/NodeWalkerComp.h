#pragma once
#include "SkeletonMeshComponent.h"
#include "Path.h"
//#include "Component.h"
//#include "Transform.h"
//#include <DirectXMath.h>
//#include "Object.h"
#include "Input.h"

constexpr float HEIGHT = 1.f; //height from ground
const float OFFSET = CHUNK_SIZE / 2.0f;

namespace dx = DirectX;
class NodeWalkerComp :public Component
{
public:
	//struct Node
	//{
	//	std::string name;
	//	int id;
	//	dx::XMFLOAT3 position;
	//	int nextMiddle;
	//	int nextLeft;
	//	int nextRight;
	//	int next;
	//	Node(std::string name, int id, dx::XMFLOAT3 position, int nextMiddle, int nextLeft, int nextRight)
	//	{
	//		this->name = name;
	//		this->id = id;
	//		this->position = position;
	//		this->nextMiddle = nextMiddle;
	//		this->nextLeft = nextLeft;
	//		this->nextRight = nextRight;
	//	}
	//};
private:
	//std::vector<Node> nodes;
	Path thePath;
	float speed;
	int currentNode;
	int nextChosen;
	float nodeRadius;
	bool canWalk;
	float length; //length is only saved for displaying
	SkeletonMeshComponent* base;
	SkeletonMeshComponent* legs;
public:
	NodeWalkerComp();
	virtual ~NodeWalkerComp();
	void InitializePath(Path thePath);
	void InitAnimation();
	void Reset(); //is public cus is meant to be called outside of class if needed
	void Start();
	void Stop();

	void Update(const float& deltaTime);
	//void InsertNode(std::string name, int id, dx::XMFLOAT3 position, int nextMiddle, int nextLeft, int nextRight);
	//void InsertNode(const Node &theNode);
	//void InsertNodes(std::vector<Node> someNodes);

};

