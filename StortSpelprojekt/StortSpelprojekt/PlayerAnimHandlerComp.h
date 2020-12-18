#pragma once
//#include "SkeletonMeshComponent.h"
//#include "Mesh.h"
//#include "Material.h"
//#include "ControllerComp.h"
#include "DXHelper.h"
class ControllerComp;
class SkeletonMeshComponent;
class PlayerComp;

class PlayerAnimHandlerComp : public Component
{
public:
	PlayerAnimHandlerComp(SkeletonMeshComponent* skeletonMeshComp, Object* camObject, Object* playerObject);
	virtual~PlayerAnimHandlerComp();

	void Initialize();
	void Update(const float& deltaTime);

	void Animate(const float& time);
	void SetPosition();
	void SetStarted(bool started) { this->gameStarted = started; }
	float GetCooldown() { return this->attackCooldown; }

private:
	CameraComponent* camComp;
	ControllerComp* controlComp;
	PlayerComp* pComponent;

	Object* camObject;
	Object* playerObject;
	SkeletonMeshComponent* skeletonMeshComp;

	bool attacking;
	float attackTimer = 0;
	float attackCooldown = 0;
	bool gameStarted = false;

	//Blending
	float finalFactor = 0.f;
	float factorValue = 0.f;
	float factorRange = 0.f;
	float playerMoveSpeed = 0;
	float sprintSpeedFactor = 0;
	float walkSpeedFactor = 0;
};