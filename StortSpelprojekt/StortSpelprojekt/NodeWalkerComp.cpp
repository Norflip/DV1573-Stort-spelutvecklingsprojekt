#include "NodeWalkerComp.h"
#include "PlayerComp.h"

NodeWalkerComp::NodeWalkerComp()
{
	this->speed = 3.0f;// 16.2f;
	this->currentNode = thePath.GetFirstPointIndex();
	//this->nextChosen = this->currentNode + 1;
	this->nodeRadius = 0.3f;
	this->canWalk = false;
	this->isWalking = true;
	this->length = 0.f;
	this->moveVec = { 0,0,0 };
}

NodeWalkerComp::~NodeWalkerComp()
{
}

void NodeWalkerComp::InitializePath(Path thePath)
{
	this->rbComp = GetOwner()->GetComponent<RigidBodyComponent>();
	this->thePath = thePath;

	this->currentNode = thePath.GetFirstPointIndex();
	this->isWalking = true;
	this->canWalk = true;
	//std::cout <<"Nr of: "<< thePath.CountPoints() << std::endl;
	//dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x + offset,HEIGHT, thePath.GetPoint(this->currentNode).y + offset };
	dx::XMFLOAT3 pos = { thePath.GetPoint(this->currentNode).x,HEIGHT, thePath.GetPoint(this->currentNode).z };
	dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos);
	
	this->rbComp->SetPosition(startPos);
	this->GetOwner()->GetTransform().SetPosition(startPos);

	this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setFrictionCoefficient(100.f);
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setRollingResistance(100.f);
	
}

void NodeWalkerComp::InitAnimation()
{
	if (GetOwner()->HasComponent<SkeletonMeshComponent>())
	{
		this->base = GetOwner()->GetComponent<SkeletonMeshComponent>();
		this->legs = GetOwner()->GetChild(0)->GetComponent<SkeletonMeshComponent>();
	}
	else
	{
		this->base = nullptr;
		this->legs = nullptr;
	}
	//Start();
	//canWalk = true;
}

//void NodeWalkerComp::Reset()
//{
//	//std::cout<<this->currentNode<< std::endl;
//	this->currentNode = thePath.GetFirstPointIndex();
//	isWalking = true;
//	//std::cout << this->nextChosen << std::endl;
//	//this->nextChosen = this->currentNode + 1;
//	//std::cout << this->canWalk << std::endl;
//
//	//dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x,HEIGHT, thePath.GetPoint(this->currentNode).z };
//	//dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos3);
//	//this->GetOwner()->GetTransform().SetPosition(startPos);
//	//this->rbComp->SetPosition(startPos);
//}

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
	dx::XMVECTOR posVec = GetOwner()->GetTransform().GetPosition();
	dx::XMFLOAT3 pos;
	dx::XMStoreFloat3(&pos, posVec);
	if (pos.y < 0.0f || pos.y > 30)//|| y < 0.0f || y > 30)
	{
		std::cout << "pos.y: " << pos.y << std::endl
			<<"fixing house pos...."<<std::endl; //", y: " << y << std::endl;
		
		pos.y = 0.01f;
		posVec = dx::XMLoadFloat3(&pos);
		rbComp->SetPosition(posVec);
		GetOwner()->GetTransform().SetPosition(posVec);
	}
	
	if (playerComp->GetFuel() > 0.0f)
	{
		if (base->SetAndGetDoneUp())
		{
			//canWalk = true;
			base->SetTrack(SkeletonStateMachine::WALK, false);
			legs->SetTrack(SkeletonStateMachine::WALK, false);
		}
		if (base->SetAndGetDoneDown())
		{
			base->SetTrack(SkeletonStateMachine::IDLE, false);
			legs->SetTrack(SkeletonStateMachine::IDLE, false);
		}
	
		if (isWalking)
		{
			if (canWalk)
			{
				dx::XMFLOAT3 nextPoint = { thePath.GetPoint(this->currentNode).x, HEIGHT, thePath.GetPoint(this->currentNode).z };
				dx::XMVECTOR vdir = dx::XMVectorSubtract(dx::XMLoadFloat3(&nextPoint), GetOwner()->GetTransform().GetPosition());
				dx::XMStoreFloat(&this->length, dx::XMVector3Length(vdir));

				if (this->length < nodeRadius)
				{
					//StopAnim();
					canWalk = false;
					this->currentNode += 1;// this->nextChosen;
					MetaProgress::Instance().SetNodeReached(currentNode);
					
				}
				else
				{
					vdir = dx::XMVector3Normalize(vdir);
					vdir = dx::XMVectorScale(vdir, speed * deltaTime);
					dx::XMStoreFloat3(&moveVec, vdir);
				
					dx::XMFLOAT3 dirToRoad;
					dx::XMStoreFloat3(&dirToRoad, vdir);

					float angle = -90.0f + atan2f(dirToRoad.x, dirToRoad.z) * (180.f / Math::PI);
					float rotation = angle * Math::ToRadians;
					dx::XMVECTOR right = GetOwner()->GetTransform().TransformDirection({ 1,0,0 });
					dx::XMVECTOR targetRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, rotation));
					dx::XMVECTOR newRotation = dx::XMQuaternionSlerp(GetOwner()->GetTransform().GetWorldRotation(), targetRotation, deltaTime);

					GetOwner()->GetTransform().SetRotation(newRotation);
					GetOwner()->GetTransform().Translate(moveVec.x, moveVec.y, moveVec.z);
				}
			}
			else
			{
				const int skip = 1;
				if (currentNode /*this->nextChosen*/ < ICAST(this->thePath.GetLastPointIndex()))
				{
					//this->nextChosen = currentNode + skip; //skip is 1
					//currentNode = currentNode + skip; //skip is 1
					//StartAnim();
					//std::cout << "next node..." << std::endl;
					canWalk = true;
				}

			}
		}
	}
	else
	{
		base->SetTrack(SkeletonStateMachine::IDLE, false);
		legs->SetTrack(SkeletonStateMachine::IDLE, false);
	}

	//if (KEY_DOWN(I)) //used to display info and test paths
	//{
	//	std::cout << "Current Node: " << std::to_string(this->currentNode) << std::endl
	//		//<< "Next Node: " << std::to_string(nextChosen) << std::endl
	//		<< "Last Node in system: " << std::to_string(this->thePath.GetLastPointIndex()) << std::endl
	//		<< "Can Walk: " << canWalk << std::endl
	//		<< "Is Walking: " << isWalking << std::endl
	//		<< "Length: " << std::to_string(this->length) << std::endl;

	//}

	//if (KEY_DOWN(R))
	//	this->Reset();
}

dx::XMFLOAT3 NodeWalkerComp::GetMoveVec()
{
	return this->moveVec;
}

//bool NodeWalkerComp::GetHouseInGoal()
//{
//	bool result = false;
//	//std::cout <<"current: "<< this->currentNode <<", last: "<< this->thePath.GetLastPointIndex() << std::endl;
//	if (this->currentNode == this->thePath.GetLastPointIndex())
//	{
//		result = true;
//		//std::cout << "goal" << std::endl;
//	}
//
//	return result;
//}

float NodeWalkerComp::GetHouseProgress()
{
	float result = 0.f;
	if(this->thePath.GetLastPointIndex()!=0)
		result = float(this->currentNode) / float(this->thePath.GetLastPointIndex());

	return result;
}

//dx::XMFLOAT3 NodeWalkerComp::GetLastPos()
//{
//	return this->lastPos;
//}
