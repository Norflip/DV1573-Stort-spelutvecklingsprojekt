#include "stdafx.h"
#include "PlayerAnimHandlerComp.h"
#include "ControllerComp.h"
#include "SkeletonMeshComponent.h"
PlayerAnimHandlerComp::PlayerAnimHandlerComp(SkeletonMeshComponent* skeletonMeshComp, Object* object, Object* playerObject)
	:skeletonMeshComp(skeletonMeshComp), camObject(object), playerObject(playerObject)
{
}

PlayerAnimHandlerComp::~PlayerAnimHandlerComp()
{
}

void PlayerAnimHandlerComp::Initialize()
{
	//skeletonMeshComp->SetTrack(SkeletonStateMachine::IDLE, false);
	camComp = camObject->GetComponent<CameraComponent>();
	controlComp = playerObject->GetComponent<ControllerComp>();
	up = { 0.0f, 1.0f, 1.0f };
}

void PlayerAnimHandlerComp::Update(const float& deltaTime)
{
	Animate(deltaTime);
	SetPosition();
}

void PlayerAnimHandlerComp::Animate(float time)
{
	attackTimer += time;
	attackCooldown += time;

	if (LMOUSE_PRESSED && attackCooldown > 1.0f)
	{
		skeletonMeshComp->SetTrack(SkeletonStateMachine::ATTACK, true); //OFÄRDIGT: ATTACKEN BUGGAR >_>
		time = 0;
		attackTimer = 0;
		attackCooldown = 0;
		attacking = true;
	}
	else if (attackTimer > 0.83f)
	{
		attacking = false;
	}

	if (attacking)
	{
		//skeletonMeshComp->SetTrack(SkeletonStateMachine::ATTACK, true); //OFÄRDIGT: ATTACKEN BUGGAR >_>
		//AudioMaster::Instance().PlaySoundEvent("axeSwing");
	}

	else if(!attacking)
	{
		if (controlComp->GetrbComp()->GetLinearVelocity().x > 0 || controlComp->GetrbComp()->GetLinearVelocity().x < 0 ||
			controlComp->GetrbComp()->GetLinearVelocity().z > 0 || controlComp->GetrbComp()->GetLinearVelocity().z < 0)
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

		else if (controlComp->GetrbComp()->GetLinearVelocity().x == 0 && controlComp->GetrbComp()->GetLinearVelocity().z == 0)
		{
			skeletonMeshComp->SetTrack(SkeletonStateMachine::IDLE, false);
		}
	}
}

void PlayerAnimHandlerComp::SetPosition()
{
	inverseViewMatrix = dx::XMMatrixInverse(nullptr, camComp->GetViewMatrix());
	armsOffTrans = dx::XMMatrixTranslation(0.0f, -1.8f, 0.0f);
	armsOffRot = dx::XMMatrixRotationAxis(up, dx::XMConvertToRadians(0.0f));
	armsWorld = armsOffRot * armsOffTrans * inverseViewMatrix;
	dx::XMMatrixDecompose(&armsScale, &armsRot, &armsPos, armsWorld);

	GetOwner()->GetTransform().SetPosition(armsPos);
	GetOwner()->GetTransform().SetRotation(armsRot);
	GetOwner()->GetTransform().SetScale(armsScale);
}
