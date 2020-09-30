#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>
#include "Object.h"
#include "Input.h"

namespace dx = DirectX;
class NodeWalker :Component
{
private:
	struct Node
	{
		std::string name;
		int id;
		dx::XMFLOAT3 position;
		int nextMiddle;
		int nextLeft;
		int nextRight;
		Node(std::string name, int id, dx::XMFLOAT3 position, int nextMiddle, int nextLeft, int nextRight)
		{
			this->name = name;
			this->id = id;
			this->position = position;
			this->nextMiddle = nextMiddle;
			this->nextLeft = nextLeft;
			this->nextRight = nextRight;
		}
		//bool operator=(const Node& other)
		//{
		//}
	};
	//fork path
	std::vector<Node> nodes;

	float speed;
	float currentNode;
	float nextChosen;
	float nodeRadius;
	float canWalk;
public:
	NodeWalker();
	~NodeWalker();

	bool Start();
	bool Stop();

	void Update(const float& deltaTime);
	void InsertNode(const dx::XMFLOAT3 nodePos);

private:

};

