#include "NodeWalkerComp.h"

NodeWalkerComp::NodeWalkerComp()
{
	this->speed = 1.0f;// 16.2f;
	this->currentNode = thePath.GetFirstPointIndex();
	this->nextChosen = -1;
	this->nodeRadius = 0.3f;
	this->canWalk = false;
	this->isWalking = true;
	this->length = 0.f;
	//this->lastPos = { 0,0,0 };
	this->moveVec = { 0,0,0 };
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

	this->currentNode = thePath.GetFirstPointIndex();

	//std::cout <<"Nr of: "<< thePath.CountPoints() << std::endl;
	//dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x + offset,HEIGHT, thePath.GetPoint(this->currentNode).y + offset };
	dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x,HEIGHT, thePath.GetPoint(this->currentNode).z };
	dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos3);
	this->GetOwner()->GetTransform().SetPosition(startPos);

	this->rbComp = GetOwner()->GetComponent<RigidBodyComponent>();
	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setFrictionCoefficient(100.f);
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setRollingResistance(100.f);
	this->rbComp->SetPosition(startPos);
	
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
	//Start();
	//canWalk = true;
}

void NodeWalkerComp::Reset()
{
	this->currentNode = thePath.GetFirstPointIndex();
	this->nextChosen = -1;
	this->canWalk = false;

	dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x,HEIGHT, thePath.GetPoint(this->currentNode).z };
	dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos3);
	this->GetOwner()->GetTransform().SetPosition(startPos);
	this->rbComp->SetPosition(startPos);
}

void NodeWalkerComp::Start()
{
	//std::cout << "start <" << std::endl;
	isWalking = true;
	StartAnim();
}

void NodeWalkerComp::Stop()
{
	//std::cout << "stop <" << std::endl;
	isWalking = false;
	StopAnim();
}

void NodeWalkerComp::StartAnim()
{
	//if (canWalk)
	//{
		//std::cout << "start Animation<" << std::endl;
		base->SetisDone(false);
		legs->SetisDone(false);
		base->SetAndGetDoneDown() = false;
		legs->SetAndGetDoneDown() = false;
		base->SetAndGetDoneUp() = false;
		legs->SetAndGetDoneUp() = false;
		base->SetTrack(SkeletonStateMachine::UP, true);
		legs->SetTrack(SkeletonStateMachine::UP, true);
	//}
}

void NodeWalkerComp::StopAnim()
{
	//if (canWalk)
	//{
		//std::cout << "stop Animation<" << std::endl;
		base->SetisDone(false);
		legs->SetisDone(false);
		base->SetTrack(SkeletonStateMachine::DOWN, true);
		legs->SetTrack(SkeletonStateMachine::DOWN, true);
		base->SetAndGetDoneDown() = false;
		legs->SetAndGetDoneDown() = false;
		base->SetAndGetDoneUp() = false;
		legs->SetAndGetDoneUp() = false;
	//}
}

void NodeWalkerComp::Update(const float& deltaTime)
{
	//dx::XMStoreFloat3(&this->lastPos, GetOwner()->GetTransform().GetPosition());
	if (base->SetAndGetDoneUp())
	{
		//canWalk = true;
		base->SetTrack(SkeletonStateMachine::WALK, false);
		legs->SetTrack(SkeletonStateMachine::WALK, false);
	}

	if (KEY_DOWN(I)) //used to display info and test paths
	{
		std::cout << "Current Node: (" << std::to_string(this->currentNode) << ")" << std::endl
			<< "Next Node: " << std::to_string(nextChosen) << std::endl
			<< "Can Walk: " << canWalk << std::endl
			<< "Length: " << std::to_string(this->length) << std::endl;

	}

	if (KEY_DOWN(R))
		this->Reset();

	if (isWalking)
	{
		if (canWalk)
		{
			//DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
			dx::XMFLOAT3 nextPoint = { thePath.GetPoint(this->currentNode).x,HEIGHT, thePath.GetPoint(this->currentNode).z  };
			dx::XMVECTOR vdir = dx::XMVectorSubtract(dx::XMLoadFloat3(&nextPoint), GetOwner()->GetTransform().GetPosition());
			dx::XMStoreFloat(&this->length, dx::XMVector3Length(vdir));
			if (this->length < nodeRadius)
			{
				StopAnim();
				canWalk = false;
				this->currentNode = this->nextChosen;
			}
			else
			{
				vdir = dx::XMVector3Normalize(vdir);
				vdir = dx::XMVectorScale(vdir, speed * deltaTime);
				dx::XMStoreFloat3(&moveVec, vdir);
				//this->moveVec = dir;

				GetOwner()->GetTransform().Translate(moveVec.x, moveVec.y, moveVec.z);
				this->rbComp->SetPosition(GetOwner()->GetTransform().GetWorldPosition());
			}
		}
		else
		{
			const int skip = 1;
			if (this->nextChosen <= ICAST(this->thePath.GetLastPointIndex()))
			{
				this->nextChosen = currentNode + skip; //skip is 10
				StartAnim();
				canWalk = true;

			}

		}
	}
}

dx::XMFLOAT3 NodeWalkerComp::GetMoveVec()
{
	return this->moveVec;
}

//dx::XMFLOAT3 NodeWalkerComp::GetLastPos()
//{
//	return this->lastPos;
//}
