#include "stdafx.h"
#include "PlayerAnimHandlerComp.h"
#include "ControllerComp.h"
#include "SkeletonMeshComponent.h"
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
}

void PlayerAnimHandlerComp::Update(const float& deltaTime)
{
	Animate(deltaTime);
	SetPosition();
}

//Handles the players animation controlls. 
void PlayerAnimHandlerComp::Animate(const float& time)
{
	attackTimer += time;
	attackCooldown += time;

	if (LMOUSE_PRESSED && attackCooldown > 1.0f)
	{
		skeletonMeshComp->SetTrack(SkeletonStateMachine::ATTACK, true);
		attackTimer = 0;
		attackCooldown = 0;
		attacking = true;
	}
	else if (attacking && attackTimer > 0.83f)
	{
		attacking = false;
	}

	else if(!attacking)
	{
		if (controlComp->GetRigidBodyComp()->GetLinearVelocity().x > 0 || controlComp->GetRigidBodyComp()->GetLinearVelocity().x < 0 ||
			controlComp->GetRigidBodyComp()->GetLinearVelocity().z > 0 || controlComp->GetRigidBodyComp()->GetLinearVelocity().z < 0)
		{
			if (KEY_PRESSED(LeftShift))
			{
				skeletonMeshComp->SetTrack(SkeletonStateMachine::RUN, false);
			}
			else
			{
				skeletonMeshComp->SetTrack(SkeletonStateMachine::WALK, false);
			}
		}

		else if (controlComp->GetRigidBodyComp()->GetLinearVelocity().x == 0 && controlComp->GetRigidBodyComp()->GetLinearVelocity().z == 0)
		{
			skeletonMeshComp->SetTrack(SkeletonStateMachine::IDLE, false);
		}
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
