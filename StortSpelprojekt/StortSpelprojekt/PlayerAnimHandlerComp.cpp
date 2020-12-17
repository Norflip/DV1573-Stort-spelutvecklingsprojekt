#include "stdafx.h"
#include "PlayerAnimHandlerComp.h"
#include "ControllerComp.h"
#include "SkeletonMeshComponent.h"
#include "PlayerComp.h"
PlayerAnimHandlerComp::PlayerAnimHandlerComp(SkeletonMeshComponent* skeletonMeshComp, Object* object, Object* playerObject)
	:skeletonMeshComp(skeletonMeshComp), camObject(object), playerObject(playerObject)
{
	this->skeletonMeshComp = skeletonMeshComp;
	this->camObject = object;
	this->playerObject = playerObject;
}

PlayerAnimHandlerComp::~PlayerAnimHandlerComp()
{
}

void PlayerAnimHandlerComp::Initialize()
{
	camComp = camObject->GetComponent<CameraComponent>();
	controlComp = playerObject->GetComponent<ControllerComp>();
	pComponent = playerObject->GetComponent<PlayerComp>();
}

void PlayerAnimHandlerComp::Update(const float& deltaTime)
{
	if(gameStarted)
		Animate(deltaTime);
	SetPosition();
}

//Handles the players animation controlls. 
void PlayerAnimHandlerComp::Animate(const float& time)
{
	attackTimer += time;
	attackCooldown += time;
	skeletonMeshComp->SetBlendedAnimTime(SkeletonStateMachine::ATTACK, SkeletonStateMachine::BLENDED);
	
	if (LMOUSE_DOWN && attackCooldown > skeletonMeshComp->GetBlendedAnimTime() + 0.4f)
	{
		attacking = true;
		attackTimer = 0;
		attackCooldown = 0;

		this->factorValue = (skeletonMeshComp->GetBlendedAnimTime() - 0.f);
		this->factorRange = (1.f - 0.f);

		this->finalFactor = (((attackTimer - 0.f) * factorRange) / factorValue) + 0.f;
		finalFactor = CLAMP(finalFactor, 0.f, 1.f);

		std::cout << "Blended animtime : " << skeletonMeshComp->GetBlendedAnimTime() << std::endl;

		skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::ATTACK, SkeletonStateMachine::BLENDED, finalFactor, true);
		skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, true);

		AudioMaster::Instance().PlaySoundEvent("axeSwing");
	}

	else if (attacking && attackTimer > skeletonMeshComp->GetBlendedAnimTime())
	{
		attacking = false;
	}

	else if (!attacking)
	{

		//this->factorValue = (1.f - 0.f);
		//this->factorRange = (1.f - 0.f);
		//
		//this->finalFactor = (((pComponent->GetMovementspeed() - 0.f) * factorRange) / factorValue) + 0.f;
		//finalFactor = CLAMP(finalFactor, 0.f, 1.f);

		//std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
		//skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::IDLE, finalFactor, true);
		//skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);


		//this->speedFactor = CLAMP(speedFactor, 0.f, 1.f);
		//if (KEY_PRESSED(LeftShift))
		//{
		//	this->speedFactor += 0.2;

		//	this->factorValue = (1.f - 0.f);
		//	this->factorRange = (1.f - 0.f);
		//	this->playerMoveSpeed = pComponent->GetMovementspeed() * (1 - this->speedFactor) + pComponent->GetSprintSpeed() * this->speedFactor;
		//	this->finalFactor = (((playerMoveSpeed - 0.f) * factorRange) / factorValue) + 0.f;
		//	finalFactor = CLAMP(finalFactor, 0.f, 1.f);


		//	std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
		//	skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::IDLE, finalFactor, true);
		//	skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);

		//	
		//}
		//else
		//{
		//	this->speedFactor -= 0.2f;
		//}

		
		//fixa nå med velociteten

		////////////////////////////////////////////////////////////////

		/*this->factorValue = (1.f - 0.f);
		this->factorRange = (1.f - 0.f);

		this->walkSpeedFactor = CLAMP(walkSpeedFactor, 0.f, 1.f);
		if (KEY_PRESSED(W) || KEY_PRESSED(A) || KEY_PRESSED(S) || KEY_PRESSED(D))
		{
			this->walkSpeedFactor += 0.2f;
			this->playerMoveSpeed = pComponent->GetMovementspeed() * (1 - this->walkSpeedFactor) + pComponent->GetSprintSpeed() * this->walkSpeedFactor;
		}
		else
		{
			this->walkSpeedFactor -= 0.2f;
		}



		this->sprintSpeedFactor = CLAMP(sprintSpeedFactor, 0.f, 1.f);
		if (KEY_PRESSED(LeftShift))
		{
			this->sprintSpeedFactor += 0.2;
			this->playerMoveSpeed = pComponent->GetMovementspeed() * (1 - this->sprintSpeedFactor) + pComponent->GetSprintSpeed() * this->sprintSpeedFactor;
		}
		else
		{
			this->sprintSpeedFactor -= 0.2f;
		}


		this->finalFactor = (((playerMoveSpeed - 0.f) * factorRange) / factorValue) + 0.f;
		finalFactor = CLAMP(finalFactor, 0.f, 1.f);

		std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
		skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::IDLE, finalFactor, true);
		skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);*/

		/////////////////////////////////////////////////////////////////

		this->factorValue = (4.f - 0.f);
		this->factorRange = (1.f - 0.f);

		this->finalFactor = (((controlComp->GetVelocity() - 0.f) * factorRange) / factorValue) + 0.f;
		finalFactor = CLAMP(finalFactor, 0.f, 1.f);

		std::cout << "Velocity x: " << controlComp->GetVelocity() << std::endl;

		skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::IDLE, SkeletonStateMachine::WALK, finalFactor, true);
		skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);

		if (controlComp->GetVelocity() >= 4.f)
		{
			this->factorValue = (7.f - 4.f);
			this->factorRange = (1.f - 0.f);

			this->finalFactor = (((controlComp->GetVelocity() - 4.f) * factorRange) / factorValue) + 0.f;
			finalFactor = CLAMP(finalFactor, 0.f, 1.f);

			skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::RUN, finalFactor, true);
			skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);
		}
		



		////////////////////////////////////////////////////////////////
	//	if (controlComp->GetRigidBodyComp()->GetLinearVelocity().x > 0 || controlComp->GetRigidBodyComp()->GetLinearVelocity().x < 0 ||
	//		controlComp->GetRigidBodyComp()->GetLinearVelocity().z > 0 || controlComp->GetRigidBodyComp()->GetLinearVelocity().z < 0)
	//	{
	//		this->factorValue = (4.f - 0.f);
	//		this->factorRange = (1.f - 0.f);

	//		//this->playerMoveSpeed = pComponent->GetMovementspeed() * (1 - controlComp->GetRigidBodyComp()->GetLinearVelocity().x) + pComponent->GetSprintSpeed() * controlComp->GetRigidBodyComp()->GetLinearVelocity().x;
	//	

	//		this->finalFactor = (((controlComp->GetVelocity() - 0.f) * factorRange) / factorValue) + 0.f;
	//		finalFactor = CLAMP(finalFactor, 0.f, 1.f);

	//		std::cout << "Velocity x: " << controlComp->GetVelocity() << std::endl;
	//		skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::IDLE, SkeletonStateMachine::WALK, finalFactor, true);
	//		skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);

	///*		if (KEY_PRESSED(LeftShift))
	//		{
	//			skeletonMeshComp->SetTrack(SkeletonStateMachine::RUN, false);
	//		}
	//		else
	//		{
	//			skeletonMeshComp->SetTrack(SkeletonStateMachine::WALK, false);
	//		}*/
	//	}
	//	
		/*if (controlComp->GetRigidBodyComp()->GetLinearVelocity().x > 0)
		{
			this->factorValue = (1.f - 0.f);
			this->factorRange = (1.f - 0.f);
			this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().x - 0.f) * factorRange) / factorValue) + 0.f;

			finalFactor = CLAMP(finalFactor, 0.f, 1.f);

			std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
			skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::RUN, SkeletonStateMachine::WALK, finalFactor, true);
			skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);

		}
		else if (controlComp->GetRigidBodyComp()->GetLinearVelocity().z > 0)
		{
			this->factorValue = (1.f - 0.f);
			this->factorRange = (1.f - 0.f);
			this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().z - 0.f) * factorRange) / factorValue) + 0.f;

			finalFactor = CLAMP(finalFactor, 0.f, 1.f);

			std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
			skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::RUN, SkeletonStateMachine::WALK, finalFactor, true);
			skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);
		}
		else if (controlComp->GetRigidBodyComp()->GetLinearVelocity().x < 0)
		{
			this->factorValue = (1.f - 0.f);
			this->factorRange = (1.f - 0.f);
			this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().x - 0.f) * factorRange) / factorValue) + 0.f;

			finalFactor = CLAMP(finalFactor, 0.f, 1.f);

			std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
			skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::RUN, finalFactor, true);
			skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);
		}
		else if (controlComp->GetRigidBodyComp()->GetLinearVelocity().z < 0)
		{
			this->factorValue = (1.f - 0.f);
			this->factorRange = (1.f - 0.f);
			this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().z - 0.f) * factorRange) / factorValue) + 0.f;

			finalFactor = CLAMP(finalFactor, 0.f, 1.f);

			std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
			skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::RUN, finalFactor, true);
			skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);
		}*/

	/*	else
		{
			skeletonMeshComp->SetTrack(SkeletonStateMachine::IDLE, false);
		}*/
	}


	
}

//Makes the players arms follow the camera.
void PlayerAnimHandlerComp::SetPosition()
{
	dx::XMVECTOR armsScale;
	dx::XMVECTOR armsRot;
	dx::XMVECTOR armsPos;
	dx::XMMATRIX inverseViewMatrix = dx::XMMatrixInverse(nullptr, camComp->GetViewMatrix());
	dx::XMMATRIX armsOffTrans = dx::XMMatrixTranslation(0.0f, -1.8f, 0.0f);
	dx::XMMATRIX armsOffRot = dx::XMMatrixRotationAxis(dx::XMVECTOR{0.0f, 1.0f, 1.0f}, dx::XMConvertToRadians(0.0f));
	dx::XMMATRIX armsWorld = armsOffRot * armsOffTrans * inverseViewMatrix;
	dx::XMMatrixDecompose(&armsScale, &armsRot, &armsPos, armsWorld);

	GetOwner()->GetTransform().SetPosition(armsPos);
	GetOwner()->GetTransform().SetRotation(armsRot);
	GetOwner()->GetTransform().SetScale(armsScale);
}


/*
if (controlComp->GetRigidBodyComp()->GetLinearVelocity().x > 0)
		{

			this->factorValue = (1.f - 0.f);
			this->factorRange = (1.f - 0.f);
			this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().x - 0.f) * factorRange) / factorValue) + 0.f;

			finalFactor = CLAMP(finalFactor, 0.f, 1.f);

			std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
			skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::RUN, SkeletonStateMachine::WALK, finalFactor, true);
			skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);

		}
		else if (controlComp->GetRigidBodyComp()->GetLinearVelocity().z > 0)
		{
		this->factorValue = (1.f - 0.f);
		this->factorRange = (1.f - 0.f);
		this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().z - 0.f) * factorRange) / factorValue) + 0.f;

		finalFactor = CLAMP(finalFactor, 0.f, 1.f);

		std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
		skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::RUN, SkeletonStateMachine::WALK, finalFactor, true);
		skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);
		}
		else if (controlComp->GetRigidBodyComp()->GetLinearVelocity().x < 0)
		{
		this->factorValue = (1.f - 0.f);
		this->factorRange = (1.f - 0.f);
		this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().x - 0.f) * factorRange) / factorValue) + 0.f;

		finalFactor = CLAMP(finalFactor, 0.f, 1.f);

		std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
		skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::RUN, finalFactor, true);
		skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);
		}
		else if (controlComp->GetRigidBodyComp()->GetLinearVelocity().z < 0)
		{
		this->factorValue = (1.f - 0.f);
		this->factorRange = (1.f - 0.f);
		this->finalFactor = (((controlComp->GetRigidBodyComp()->GetLinearVelocity().z - 0.f) * factorRange) / factorValue) + 0.f;

		finalFactor = CLAMP(finalFactor, 0.f, 1.f);

		std::cout << "linear velocity x: " << controlComp->GetRigidBodyComp()->GetLinearVelocity().x << std::endl;
		skeletonMeshComp->SetBlendingTracksAndFactor(SkeletonStateMachine::WALK, SkeletonStateMachine::RUN, finalFactor, true);
		skeletonMeshComp->SetTrack(SkeletonStateMachine::BLENDED, false);
		}

*/