#include "WeaponComponent.h"

WeaponComponent::WeaponComponent(Object* object, Input& input)
	:camObj(object), weaponPos({ 0,0,0 }), weaponRot({ 0,0,0 }), weaponScale({ 0,0,0 }), up({ 0,0,0 }), wepInput(&input), attacking(false)
{
}

void WeaponComponent::Initialize()
{
}

void WeaponComponent::Update(const float& deltaTime)
{
	SetPosition(deltaTime);
}

void WeaponComponent::SetPosition(float time)
{

	up = { 0.0f, 1.0f, 1.0f };
	inverseViewMatrix = dx::XMMatrixInverse(nullptr, camObj->GetComponent<CameraComponent>()->GetViewMatrix());
	wepOffTrans = dx::XMMatrixTranslation(0.3f, -0.2f, 0.8f);
	wepOffRot = dx::XMMatrixRotationAxis(up, dx::XMConvertToRadians(-40.0f));


	//TA BORT SEN EFTER SPELTEST/////////////////////
	// ATTACK LOGIC - PLACEHOLDER AS FUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
	attackTimer += time;
	attackCooldown += time;
	
	if (wepInput->GetRightMouseKeyDown() && attackCooldown > 0.5)
	{
		time = 0;
		attackTimer = 0;
		attackCooldown = 0;
		attacking = true;
	}
	else if (attackTimer > 0.2f)
	{
		attacking = false;
	}
	if (attacking)
	{
		wepOffTrans = dx::XMMatrixTranslation(0.1f, -0.1f, 1.1f);
		wepOffRot = dx::XMMatrixRotationAxis({ -1.0f,1.0f,0.5f }, dx::XMConvertToRadians(-60.0f));
	}
	///////////////////////////////////////////////////////////////////////


	
	wepWorld = wepOffRot * wepOffTrans * inverseViewMatrix;
	dx::XMMatrixDecompose(&weaponScale, &weaponRot, &weaponPos, wepWorld);

	GetOwner()->GetTransform().SetPosition(weaponPos);
	GetOwner()->GetTransform().SetRotation(weaponRot);
	GetOwner()->GetTransform().SetScale(weaponScale);

}
