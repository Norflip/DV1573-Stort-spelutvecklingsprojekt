#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>
#include "Object.h"
#include "Input.h"

namespace dx = DirectX;
class NodeWalker :public Component
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
	};
	std::vector<Node> nodes;

	float speed;
	int currentNode;
	int nextChosen;
	float nodeRadius;
	bool canWalk;
	float length; //length is only saved for display with
public:
	NodeWalker();
	~NodeWalker();

	void Reset();
	void Start();
	void Stop();

	void Update(const float& deltaTime);
	void InsertNode(const dx::XMFLOAT3 nodePos);

private:

};

