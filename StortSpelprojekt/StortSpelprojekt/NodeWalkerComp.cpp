#include "NodeWalkerComp.h"
#include "PlayerComp.h"

NodeWalkerComp::NodeWalkerComp()
{
	this->speed = 10.f;// 3.0f;// 16.2f;
	this->currentNode = thePath.GetFirstPointIndex();
	this->nextChosen = this->currentNode + 1;
	this->nodeRadius = 0.3f;
	this->canWalk = false;
	this->isWalking = true;
	this->length = 0.f;
	//this->lastPos = { 0,0,0 };
	this->moveVec = { 0,0,0 };
	this->pos3 = { 0,0,0 };
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
	pos3 = { thePath.GetPoint(this->currentNode).x,HEIGHT, thePath.GetPoint(this->currentNode).z };
	dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos3);
	this->GetOwner()->GetTransform().SetPosition(startPos);

	this->rbComp = GetOwner()->GetComponent<RigidBodyComponent>();
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setBounciness(0.f);
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setFrictionCoefficient(100.f);
	//this->rbComp->GetRigidBody()->getCollider(0)->getMaterial().setRollingResistance(100.f);
	this->rbComp->SetPosition(startPos);
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

void NodeWalkerComp::Reset()
{
	//std::cout<<this->currentNode<< std::endl;
	this->currentNode = thePath.GetFirstPointIndex();
	//std::cout << this->nextChosen << std::endl;
	this->nextChosen = this->currentNode + 1;
	//std::cout << this->canWalk << std::endl;

	//dx::XMFLOAT3 pos3 = { thePath.GetPoint(this->currentNode).x,HEIGHT, thePath.GetPoint(this->currentNode).z };
	//dx::XMVECTOR startPos = dx::XMLoadFloat3(&pos3);
	//this->GetOwner()->GetTransform().SetPosition(startPos);
	//this->rbComp->SetPosition(startPos);
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

void NodeWalkerComp::GetPlayerInfo(PlayerComp* playerComp)
{
	this->playerComp = playerComp;
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
				float y = this->world->SampleHeight(thePath.GetPoint(this->currentNode).x, thePath.GetPoint(this->currentNode).z);
				if (y < 0.0f || y > 30)
				{
					y = 0.01f;
					dx::XMVECTOR forcedPosVec = GetOwner()->GetTransform().GetPosition();
					dx::XMFLOAT3 forcedPos;
					dx::XMStoreFloat3(&forcedPos, forcedPosVec);
					forcedPos.y = y;
					forcedPosVec = dx::XMLoadFloat3(&forcedPos);
					rbComp->SetPosition(forcedPosVec);
					GetOwner()->GetTransform().SetPosition(forcedPosVec);
				}
				//DirectX::XMFLOAT3 dir = { 0.f,0.f,0.f };
				dx::XMFLOAT3 nextPoint = { thePath.GetPoint(this->currentNode).x, y, thePath.GetPoint(this->currentNode).z };
				dx::XMVECTOR vdir = dx::XMVectorSubtract(dx::XMLoadFloat3(&nextPoint), GetOwner()->GetTransform().GetPosition());
				dx::XMStoreFloat(&this->length, dx::XMVector3Length(vdir));
				if (this->length < nodeRadius)
				{
					//StopAnim();
					canWalk = false;
					this->currentNode = this->nextChosen;
				}
				else
				{
					vdir = dx::XMVector3Normalize(vdir);
					vdir = dx::XMVectorScale(vdir, speed * deltaTime);
					dx::XMStoreFloat3(&moveVec, vdir);
					//this->moveVec = dir;

					// BÖRJA HÄR EMIL! :D
					////'''''''''''''''''''
					//dx::XMVECTOR dirVec = vdir;
					//dx::XMFLOAT3 dirToRoad;
					//dx::XMStoreFloat3(&dirToRoad, dirVec);
					//float angle = /*180.f*/-90.0f + atan2f(dirToRoad.x, dirToRoad.z) * (180.f / Math::PI);
					//float rotation = angle * Math::ToRadians;
					//dx::XMVECTOR right = GetOwner()->GetTransform().TransformDirection({ 1,0,0 });
					//dx::XMVECTOR eulerRotation = dx::XMQuaternionMultiply(dx::XMQuaternionRotationAxis(right, 0), dx::XMQuaternionRotationAxis({ 0,1,0 }, rotation));
					//GetOwner()->GetTransform().SetRotation(eulerRotation);
					//rbComp->SetRotation(eulerRotation);
					////'''''''''''''

					GetOwner()->GetTransform().Translate(moveVec.x, moveVec.y, moveVec.z);
					this->rbComp->SetPosition(GetOwner()->GetTransform().GetWorldPosition());
				}
			}
			else
			{
				const int skip = 1;
				if (this->nextChosen <= ICAST(this->thePath.GetLastPointIndex()))
				{
					this->nextChosen = currentNode + skip; //skip is 1
					//StartAnim();
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
	//	std::cout << "Current Node: (" << std::to_string(this->currentNode) << ")" << std::endl
	//		<< "Next Node: " << std::to_string(nextChosen) << std::endl
	//		<< "Can Walk: " << canWalk << std::endl
	//		<< "Length: " << std::to_string(this->length) << std::endl;

	//}

	if (KEY_DOWN(R))
		this->Reset();
}

dx::XMFLOAT3 NodeWalkerComp::GetMoveVec()
{
	return this->moveVec;
}

//dx::XMFLOAT3 NodeWalkerComp::GetLastPos()
//{
//	return this->lastPos;
//}
