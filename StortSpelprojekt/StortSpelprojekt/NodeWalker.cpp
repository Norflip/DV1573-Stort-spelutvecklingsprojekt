#include "NodeWalker.h"

NodeWalker::NodeWalker()
{
	this->speed = 1.f;
	this->nodeRadius = 0.2f;
	this->currentNode = 0;
	this->nextChosen = -1;
	this->nodes.push_back(Node("start", 0, { 0.f,0.f,0.f }, 1, -1, -1));
	this->nodes.push_back(Node("1a", 1, { 5.f,0.f,5.f }, -1, 2, 3));
	this->nodes.push_back(Node("2a", 2, { 10.f,0.f,15.f }, 4, -1, 5));
	this->nodes.push_back(Node("2b", 3, { 20.f,0.f,7.f }, 6, 7, 8));
	this->nodes.push_back(Node("3a", 4, { 12.f,0.f,30.f }, -1, -1, 9));
	this->nodes.push_back(Node("3b", 5, { 18.f,0.f,25.f }, 9, -1, -1));
	this->nodes.push_back(Node("3c", 6, { 26.f,0.f,20.f }, 9, -1, -1));
	this->nodes.push_back(Node("3d", 7, { 30.f,0.f,15.f }, 9, -1, -1));
	this->nodes.push_back(Node("3e", 8, { 35.f,0.f,10.f }, -1, 9, -1));
	this->nodes.push_back(Node("Final Stage", 9, { 40.f,0.f,40.f }, -1, -1, -1));


	dx::XMVECTOR startPos = dx::XMLoadFloat3(&nodes[0].position);
	this->GetOwner()->GetTransform().SetPosition(startPos);
}

NodeWalker::~NodeWalker()
{
}

bool NodeWalker::Start()
{
	canWalk = true;
	return false;
}

void NodeWalker::Update(const float& deltaTime)
{
	
	if(KEY_PRESSED(I))
	{
		std::cout << "Current Node: " << std::to_string(currentNode) << std::endl
			<< "Next Node: " << std::to_string(nextChosen) << std::endl;
	}

	if (canWalk == true)
	{
		DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
		//dx::XMVECTOR current = dx::XMLoadFloat3(&nodes[currentNode].position);
		//dx::XMVECTOR next = dx::XMLoadFloat3(&nodes[nextChosen].position);
		dx::XMVECTOR vdir = dx::XMVectorSubtract(dx::XMLoadFloat3(&nodes[nextChosen].position), dx::XMLoadFloat3(&nodes[currentNode].position));
		float length;
		dx::XMStoreFloat(&length, dx::XMVector3Length(vdir));
		if (length > nodeRadius || KEY_PRESSED(Down))
		{
			canWalk = false;
		}
		else
		{
			vdir = dx::XMVector3Normalize(vdir);
			vdir = dx::XMVectorScale(vdir, speed);
			dx::XMStoreFloat3(&dir, vdir);
			GetOwner()->GetTransform().Translate(dir.x, dir.y, dir.z);
		}
	}
	if (this->nextChosen == -1)
	{
		if (KEY_PRESSED(Left))
		{
			this->nextChosen = nodes[currentNode].nextLeft;
			canWalk = true;
		}
		if (KEY_PRESSED(Up))
		{
			this->nextChosen = nodes[currentNode].nextMiddle;
			canWalk = true;
		}
		if (KEY_PRESSED(Right))
		{
			this->nextChosen = nodes[currentNode].nextRight;
			canWalk = true;
		}
		//if (this->nextChosen != -1)
		//	canWalk = true;
	}


}

