#include "NodeWalkerComp.h"

NodeWalkerComp::NodeWalkerComp()
{
	this->speed = 1.0f;// 16.2f;
	this->currentNode = CHUNK_SIZE;
	this->nextChosen = -1;
	this->nodeRadius = 0.3f;
	this->canWalk = false;
	this->length = 0.f;

	//later generate this nodes 
	//this->nodes.push_back(Node("start", 0, { 0.f,0.f,0.f }, 1, -1, -1));
	//this->nodes.push_back(Node("1a", 1, { 5.f,0.f,5.f }, -1, 2, 3));
	//this->nodes.push_back(Node("2a", 2, { 10.f,0.f,15.f }, 4, -1, 5));
	//this->nodes.push_back(Node("2b", 3, { 20.f,0.f,7.f }, 6, 7, 8));
	//this->nodes.push_back(Node("3a", 4, { 12.f,0.f,30.f }, -1, -1, 9));
	//this->nodes.push_back(Node("3b", 5, { 18.f,0.f,25.f }, 9, -1, -1));
	//this->nodes.push_back(Node("3c", 6, { 26.f,0.f,20.f }, 9, -1, -1));
	//this->nodes.push_back(Node("3d", 7, { 30.f,0.f,15.f }, 9, -1, -1));
	//this->nodes.push_back(Node("3e", 8, { 35.f,0.f,10.f }, -1, 9, -1));
	//this->nodes.push_back(Node("Final Stage", 9, { 40.f,0.f,40.f }, -1, -1, -1));
}

NodeWalkerComp::~NodeWalkerComp()
{
}

void NodeWalkerComp::InitializePath(Path thePath)
{

	this->thePath = thePath;
	//std::cout <<"Nr of: "<< thePath.CountPoints() << std::endl;
	//dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x + offset,HEIGHT, thePath.GetPoint(this->currentNode).y + offset };
	dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x + OFFSET,HEIGHT, thePath.GetPoint(this->currentNode).y + OFFSET };
	dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos3);
	this->GetOwner()->GetTransform().SetPosition(startPos);

	if (GetOwner()->HasComponent<RigidBodyComponent>())
		GetOwner()->GetComponent<RigidBodyComponent>()->SetPosition(startPos);
}

void NodeWalkerComp::InitAnimation()
{
	if (GetOwner()->HasComponent<SkeletonMeshComponent>())
	{
		this->base = GetOwner()->GetComponent<SkeletonMeshComponent>();
		this->legs = GetOwner()->GetTransform().GetChildren()[0]->GetOwner()->GetComponent<SkeletonMeshComponent>();
	}
	else
	{
		this->base = nullptr;
		this->legs = nullptr;
	}
}

void NodeWalkerComp::Reset()
{
	this->currentNode = CHUNK_SIZE;
	this->nextChosen = -1;
	this->canWalk = false;

	dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x + OFFSET,HEIGHT, thePath.GetPoint(this->currentNode).y + OFFSET };
	dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos3);
	this->GetOwner()->GetTransform().SetPosition(startPos);
	if (GetOwner()->HasComponent<RigidBodyComponent>())
		GetOwner()->GetComponent<RigidBodyComponent>()->SetPosition(startPos);
}

void NodeWalkerComp::Start()
{
	if (!canWalk)
	{
		canWalk = true;
		base->SetisDone(false);
		legs->SetisDone(false);
		base->SetAndGetDoneDown() = false;
		legs->SetAndGetDoneDown() = false;
		base->SetAndGetDoneUp() = false;
		legs->SetAndGetDoneUp() = false;
		base->SetTrack(SkeletonStateMachine::UP, true);
		legs->SetTrack(SkeletonStateMachine::UP, true);
	}
}

void NodeWalkerComp::Stop()
{
	if (canWalk)
	{
		canWalk = false;
		base->SetisDone(false);
		legs->SetisDone(false);
		base->SetTrack(SkeletonStateMachine::DOWN, true);
		legs->SetTrack(SkeletonStateMachine::DOWN, true);
		base->SetAndGetDoneDown() = false;
		legs->SetAndGetDoneDown() = false;
		base->SetAndGetDoneUp() = false;
		legs->SetAndGetDoneUp() = false;
	}
}

void NodeWalkerComp::Update(const float& deltaTime)
{

	if (base->SetAndGetDoneUp())
	{
		//canWalk = true;
		base->SetTrack(SkeletonStateMachine::WALK, false);
		legs->SetTrack(SkeletonStateMachine::WALK, false);
	}

	//if (KEY_DOWN(I)) //used to display info and test paths
	//{
	//	std::string text = "Options: ";
	//	//if (!canWalk)
	//	//{
	//	//	if (nodes[currentNode].nextLeft != -1)
	//	//		text += "[Left]";
	//	//	if (nodes[currentNode].nextMiddle != -1)
	//	//		text += "[Middle]";
	//	//	if (nodes[currentNode].nextRight != -1)
	//	//		text += "[Right]";
	//	//}
	//	//else 
	//	//	text += "[Is walking rn..]";

	//	//std::cout << "Current Node: " << nodes[currentNode].name << " (" << std::to_string(currentNode) << ")" << std::endl
	//	//	<< "Next Node: " << std::to_string(nextChosen) << std::endl
	//	//	<< "Can Walk: " << canWalk << std::endl
	//	//	<< text << std::endl
	//	//	<< "Length: " << std::to_string(this->length) << std::endl;

	//	std::cout << "Current Node: (" << std::to_string(this->currentNode) << ")" << std::endl
	//		<< "Next Node: " << std::to_string(nextChosen) << std::endl
	//		<< "Can Walk: " << canWalk << std::endl
	//		<< text << std::endl
	//		<< "Length: " << std::to_string(this->length) << std::endl;

	//}


	if (KEY_DOWN(R))
		this->Reset();

	//if (KEY_PRESSED(Down))
	//	//canWalk = false;
	//	Stop();


	if (canWalk)
	{
		DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
		dx::XMFLOAT3 nextPoint = { thePath.GetPoint(this->currentNode).x + OFFSET,HEIGHT, thePath.GetPoint(this->currentNode).y + OFFSET };
		dx::XMVECTOR vdir = dx::XMVectorSubtract(dx::XMLoadFloat3(&nextPoint), GetOwner()->GetTransform().GetPosition());
		//dx::XMVECTOR vdir = dx::XMVectorSubtract(dx::XMLoadFloat3(&nodes[nextChosen].position), GetOwner()->GetTransform().GetPosition());
		dx::XMStoreFloat(&this->length, dx::XMVector3Length(vdir));
		if (this->length < nodeRadius)
		{
			//canWalk = false;
			Stop();
			//Start();
			this->currentNode = this->nextChosen;
		}
		else
		{
			vdir = dx::XMVector3Normalize(vdir);
			vdir = dx::XMVectorScale(vdir, speed * deltaTime);
			dx::XMStoreFloat3(&dir, vdir);

			GetOwner()->GetTransform().Translate(dir.x, dir.y, dir.z);

			if (GetOwner()->HasComponent<RigidBodyComponent>())
			{
				GetOwner()->GetComponent<RigidBodyComponent>()->SetPosition(GetOwner()->GetTransform().GetWorldPosition());
			}
		}
	}
	else
	{
		const int skip = 10;
		if (/*KEY_DOWN(Up) &&  */ this->nextChosen < (int)this->thePath.CountPoints())
		{

			this->nextChosen = currentNode + skip; //skip is 10
			//canWalk = true;
			Start();

		}

	}
}